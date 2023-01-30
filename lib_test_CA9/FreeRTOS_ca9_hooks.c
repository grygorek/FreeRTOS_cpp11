/// Copyright 2018-2023 Piotr Grygorczuk <grygorek@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.

// Implementation of functions required by FreeRTOS Cortex-A port

#include "FreeRTOS.h"
#include "ca9_global_timer.h"

#include "ARMCA9.h"
#include "irq_ctrl.h"

void vApplicationIRQHandler(unsigned long ulICCIAR)
{
  unsigned irq_num = ulICCIAR & 0x3FFUL;
  __enable_irq();
  IRQHandler_t isr = IRQ_GetHandler(irq_num);
  if (!isr)
    while (1)
      ; // zonk
  isr();
}

void vClearTickInterrupt(void)
{
  GlobTimer_IrqClr();
}

void vSetupTickInterrupt(void)
{
  void FreeRTOS_Tick_Handler(void);
  IRQ_SetHandler(GlobalTimer_IRQn, FreeRTOS_Tick_Handler);
  IRQ_Enable(GlobalTimer_IRQn);

  // random value out of nowhere
  const int clkHz = 100 * 1000 * 1000;

  GlobTimer_Init();
  GlobalTimer_IncValue(clkHz / configTICK_RATE_HZ);
  GlobTimer_CompareValue(clkHz / configTICK_RATE_HZ);
  GLOBTMR->control |= GLOBTMR_CTRL_CMP_EN | GLOBTMR_CTRL_AUTO_INC | GLOBTMR_CTRL_EN | GLOBTMR_CTRL_IRQ_EN;
}
