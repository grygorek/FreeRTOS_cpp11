//*****************************************************************************
//
// startup.c - Boot code for Stellaris.
//
// Copyright (c) 2005-2007 Luminary Micro, Inc.  All rights reserved.
//
// Software License Agreement
//
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
//
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  Any use in violation
// of the foregoing restrictions may subject the user to criminal sanctions
// under applicable laws, as well as to civil liability for the breach of the
// terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 1049 of the Stellaris Driver Library.
//
//*****************************************************************************
//
// Modified by Piotr Grygorczuk
//

#include "FreeRTOS.h"
#include "task.h"

#define WEAK __attribute__((weak))
#define WEAK_AV __attribute__((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__((weak, alias(#f)))

//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" void __libc_init_array(void);

//*****************************************************************************
//
// Hardware initialization
//
//*****************************************************************************
extern "C" void SystemInit();


//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
extern "C" __attribute__((section(".after_vectors.reset"))) void ResetISR(void);
WEAK void NmiSR(void);
WEAK void FaultISR(void);
WEAK void IntDefaultHandler(void);
extern "C" void xPortPendSVHandler(void);
extern "C" void xPortSysTickHandler(void);
extern "C" void vUART_ISR(void);
extern "C" void vGPIO_ISR(void);
extern "C" void vPortSVCHandler(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// stack defined in linker script
//
//*****************************************************************************
extern "C" void _vStackTop(void);

//*****************************************************************************
//
// The minimal vector table for a Cortex-M3.  Note that the proper constructs
// must be placed on this to ensure that it ends up at physical address
// 0x0000.0000.
//
//*****************************************************************************
__attribute__((used, section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
  &_vStackTop,                            // The initial stack pointer
  ResetISR,                               // The reset handler
  NmiSR,                                  // The NMI handler
  FaultISR,                               // The hard fault handler
  IntDefaultHandler,                      // The MPU fault handler
  IntDefaultHandler,                      // The bus fault handler
  IntDefaultHandler,                      // The usage fault handler
  0,                                      // Reserved
  0,                                      // Reserved
  0,                                      // Reserved
  0,                                      // Reserved
  vPortSVCHandler,                        // SVCall handler
  IntDefaultHandler,                      // Debug monitor handler
  0,                                      // Reserved
  xPortPendSVHandler,                     // The PendSV handler
  xPortSysTickHandler,                    // The SysTick handler
  IntDefaultHandler,                      // GPIO Port A
  IntDefaultHandler,                      // GPIO Port B
  vGPIO_ISR,								// GPIO Port C
  IntDefaultHandler,                      // GPIO Port D
  IntDefaultHandler,                      // GPIO Port E
  vUART_ISR,								// UART0 Rx and Tx
  IntDefaultHandler,                      // UART1 Rx and Tx
  IntDefaultHandler,                      // SSI Rx and Tx
  IntDefaultHandler,                      // I2C Master and Slave
  IntDefaultHandler,                      // PWM Fault
  IntDefaultHandler,                      // PWM Generator 0
  IntDefaultHandler,                      // PWM Generator 1
  IntDefaultHandler,                      // PWM Generator 2
  IntDefaultHandler,                      // Quadrature Encoder
  IntDefaultHandler,                      // ADC Sequence 0
  IntDefaultHandler,                      // ADC Sequence 1
  IntDefaultHandler,                      // ADC Sequence 2
  IntDefaultHandler,                      // ADC Sequence 3
  IntDefaultHandler,                      // Watchdog timer
  IntDefaultHandler,                      // Timer 0 subtimer A
  IntDefaultHandler,                      // Timer 0 subtimer B
  IntDefaultHandler,                      // Timer 1 subtimer A
  IntDefaultHandler,                      // Timer 1 subtimer B
  IntDefaultHandler,                      // Timer 2 subtimer A
  IntDefaultHandler,                      // Timer 2 subtimer B
  IntDefaultHandler,                      // Analog Comparator 0
  IntDefaultHandler,                      // Analog Comparator 1
  IntDefaultHandler,                      // Analog Comparator 2
  IntDefaultHandler,                      // System Control (PLL, OSC, BO)
  IntDefaultHandler                       // FLASH Control
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied main() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
static void free_rtos_main(void*)
{
  // ISO C++ forbits calling 'main' but it must be done for embedded.
  // So, suppressing warning: ISO C++ forbids taking address of function '::main'
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  main();
#pragma GCC diagnostic pop
}

__attribute__((section(".after_vectors.reset"))) void ResetISR(void)
{
  unsigned long* pulSrc, * pulDest;

  // Disable interrupts
  __asm volatile("cpsid i");

  //
  // Copy the data segment initializers from flash to SRAM.
  //
  pulSrc = &_etext;
  for (pulDest = &_data; pulDest < &_edata; )
  {
    *pulDest++ = *pulSrc++;
  }

  //
  // Zero fill the bss segment.
  //
  for (pulDest = &_bss; pulDest < &_ebss; )
  {
    *pulDest++ = 0;
  }
  //
  // Initialize Hardware registers
  //
  SystemInit();

#if defined(__cplusplus)
  //
  // Call C++ library initialisation
  //
  __libc_init_array();
#endif

  __asm volatile("cpsie i");

  //
  // Call the application's entry point.
  //
  xTaskCreate(free_rtos_main,
    "main",
    configMAIN_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 1,
    NULL);

  // Start the real time scheduler.
  vTaskStartScheduler();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
WEAK_AV void
NmiSR(void)
{
  //
  // Enter an infinite loop.
  //
  while (1)
  {
  }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
WEAK_AV void
FaultISR(void)
{
  //
  // Enter an infinite loop.
  //
  while (1)
  {
  }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
WEAK_AV void
IntDefaultHandler(void)
{
  //
  // Go into an infinite loop.
  //
  while (1)
  {
  }
}


extern "C" void _exit(int)
{
  while(1)
    ;
}
