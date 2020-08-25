/// @file
///
/// @author: Piotr Grygorczuk grygorek@gmail.com
///
/// @copyright Copyright 2019 Piotr Grygorczuk
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// o Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
///
/// o Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// o My name may not be used to endorse or promote products derived from this
///   software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_time.h"

#include <thread>
#include <mutex>
#include <string>
#include <stdio.h>
#include <cstring>
#include <chrono>

#include <assert.h>

extern "C"
{
#include "DriverLib.h"
#include "integer.h"
#include "PollQ.h"
#include "semtest.h"
#include "BlockQ.h"
}


std::mutex g_lcdProtect_mtx;

const int32_t bufferSize = 16;
volatile bool g_newText{ false };
char g_inBuffer[bufferSize];
int32_t g_len{};

using namespace std::chrono_literals;

/*-----------------------------------------------------------*/

void text_scroll()
{
  char text_buffer[bufferSize] = "Hello World";
  const int32_t lcdWidth{ 96 };
  const int32_t charWidth{ 6 };
  int32_t textLen{ sizeof("Hello World") - 1 };
  int32_t maxRightPos{ lcdWidth - textLen * charWidth - 1 };
  int32_t step{ 1 };
  int32_t pos{};


  while (1)
  {
    if (g_newText)
    { // When a new text is ready
      // copy text to print from a global to a local buffer.
      std::copy(g_inBuffer, g_inBuffer + g_len + 1, text_buffer);
      textLen = g_len;
      maxRightPos = lcdWidth - textLen * charWidth - 1;
      pos = 0;
      step = 1;
      g_newText = false;
    }

    {
      std::lock_guard<std::mutex> lg{ g_lcdProtect_mtx };

      // clear the first line
      OSRAMStringDraw("                   ", 0, 0);

      // bounce from left/right edges
      pos += step;
      if (pos == 0 || pos == maxRightPos)
        step *= -1;

      // print text
      OSRAMStringDraw(text_buffer, pos, 0);
    }

    std::this_thread::sleep_for(20ms);
  }
}

/*-----------------------------------------------------------*/

void uart_print(const char* text) {
  while (*text)
    UARTCharPut(UART0_BASE, *text++);
}

/*-----------------------------------------------------------*/

int main() {
  uart_print("Type text to show on LCD...\r\n");

  OSRAMInit(false);

  std::thread scroll{ text_scroll };

  int32_t counter{};

  while (1)
  {
    {
      std::lock_guard<std::mutex> lg{ g_lcdProtect_mtx };
      OSRAMStringDraw(std::to_string(counter).c_str(), 48, 1);
    }

    counter += 1;
    std::this_thread::sleep_for(1s);
  }
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
  /* Setup the PLL. */
  SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);

  /* Enable the UART.  */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  /* Set GPIO A0 and A1 as peripheral function.  They are used to output the
  UART signals. */
  GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW);

  /* Configure the UART for 8-N-1 operation. */
  UARTConfigSet(UART0_BASE, 19200, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE);

  /* We don't want to use the fifo.  This is for test purposes to generate
  as many interrupts as possible. */
  HWREG(UART0_BASE + UART_O_LCR_H) &= ~0x10;// no fifo;

  /* Enable Tx interrupts. */
  HWREG(UART0_BASE + UART_O_IM) |= UART_INT_RX;
  IntPrioritySet(INT_UART0, configKERNEL_INTERRUPT_PRIORITY);
  IntEnable(INT_UART0);
}
/*-----------------------------------------------------------*/

extern "C" void SystemInit()
{
  prvSetupHardware();
}
/*-----------------------------------------------------------*/

extern "C" void vUART_ISR(void)
{
  static int wrPos = 0;
  auto ulStatus = UARTIntStatus(UART0_BASE, pdTRUE);
  UARTIntClear(UART0_BASE, ulStatus);

  if (ulStatus & UART_INT_RX)
  {
    g_inBuffer[wrPos] = HWREG(UART0_BASE + UART_O_DR);
    HWREG(UART0_BASE + UART_O_DR) = g_inBuffer[wrPos]; //echo back

    if (wrPos > 0 && (g_inBuffer[wrPos] == '\n' || g_inBuffer[wrPos] == '\r'))
    {
      g_inBuffer[wrPos] = 0;
      g_newText = true;
      g_len = wrPos;
      wrPos = 0;
      return;
    }

    wrPos += 1;
    if (wrPos >= bufferSize)
      wrPos = 0;
  }
}
/*-----------------------------------------------------------*/

#define PUSH_BUTTON             GPIO_PIN_4

extern "C" void vGPIO_ISR(void)
{
  GPIOPinIntClear(GPIO_PORTC_BASE, PUSH_BUTTON);
}
/*-----------------------------------------------------------*/
