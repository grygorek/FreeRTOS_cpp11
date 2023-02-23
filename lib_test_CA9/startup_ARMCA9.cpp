/******************************************************************************
 * @file     startup_ARMCA9.c
 * @brief    CMSIS Device System Source File for Arm Cortex-A9 Device Series
 * @version  V1.00
 * @date     10. January 2018
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Modified by Piotr Grygorczuk (grygorek@gmail.com)
 * 1. Make this file cpp so we can call public 'main' from cpp file without 
 *    extern "C".
 * 2. Start main as a first thread of FreeRTOS.
 * 3. Add bss init, data section copy and cpp init.
 */

#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "ARMCA9.h"

/*----------------------------------------------------------------------------
  Definitions
 *----------------------------------------------------------------------------*/
#define USR_MODE 0x10            // User mode
#define FIQ_MODE 0x11            // Fast Interrupt Request mode
#define IRQ_MODE 0x12            // Interrupt Request mode
#define SVC_MODE 0x13            // Supervisor mode
#define ABT_MODE 0x17            // Abort mode
#define UND_MODE 0x1B            // Undefined Instruction mode
#define SYS_MODE 0x1F            // System mode

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/

extern "C" {

// Init C++ stuff
void __libc_init_array(void);

/** \brief Exception and Interrupt Handler Jumptable.
*/
void Vectors       (void) __attribute__ ((naked, section("RESET")));

/** \brief Reset Handler
*/
void Reset_Handler (void) __attribute__ ((naked));

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
void Undef_Handler (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler   (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));
void PAbt_Handler  (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));
void DAbt_Handler  (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler   (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));
void FIQ_Handler   (void) { while(1); }//__attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/
void Vectors(void) {
  __ASM volatile(
  "LDR    PC, =Reset_Handler                        \n"
  "LDR    PC, =Undef_Handler                        \n"
  "LDR    PC, =FreeRTOS_SWI_Handler                 \n"
  "LDR    PC, =PAbt_Handler                         \n"
  "LDR    PC, =DAbt_Handler                         \n"
  "NOP                                              \n"
  "LDR    PC, =FreeRTOS_IRQ_Handler                 \n"
  "LDR    PC, =FIQ_Handler                          \n"
  );
}

} // extern "C"

void data_init(unsigned int romstart, unsigned int start, unsigned int len)
{
  unsigned int *pulDest = (unsigned int *)start;
  unsigned int *pulSrc = (unsigned int *)romstart;
  unsigned int loop;
  for (loop = 0; loop < len; loop = loop + 4)
    *pulDest++ = *pulSrc++;
}

void bss_init(unsigned int start, unsigned int len)
{
  unsigned int *pulDest = (unsigned int *)start;
  unsigned int loop;
  for (loop = 0; loop < len; loop = loop + 4)
    *pulDest++ = 0;
}

// Forwared declaration
int main();

void free_rtos_main(void*) {
  exit(main());
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
extern "C" void Reset_Handler(void) {
  __ASM volatile(

  // Mask interrupts
  "CPSID   if                                      \n"

  // Put any cores other than 0 to sleep
  "MRC     p15, 0, R0, c0, c0, 5                   \n"  // Read MPIDR
  "ANDS    R0, R0, #3                              \n"
  "goToSleep:                                      \n"
  "WFINE                                           \n"
  "BNE     goToSleep                               \n"

  // Reset SCTLR Settings
  "MRC     p15, 0, R0, c1, c0, 0                   \n"  // Read CP15 System Control register
  "BIC     R0, R0, #(0x1 << 12)                    \n"  // Clear I bit 12 to disable I Cache
  "BIC     R0, R0, #(0x1 <<  2)                    \n"  // Clear C bit  2 to disable D Cache
  "BIC     R0, R0, #0x1                            \n"  // Clear M bit  0 to disable MMU
  "BIC     R0, R0, #(0x1 << 11)                    \n"  // Clear Z bit 11 to disable branch prediction
  "BIC     R0, R0, #(0x1 << 13)                    \n"  // Clear V bit 13 to disable hivecs
  "MCR     p15, 0, R0, c1, c0, 0                   \n"  // Write value back to CP15 System Control register
  "ISB                                             \n"

  // Configure ACTLR
  "MRC     p15, 0, r0, c1, c0, 1                   \n"  // Read CP15 Auxiliary Control Register
  "ORR     r0, r0, #(1 <<  1)                      \n"  // Enable L2 prefetch hint (UNK/WI since r4p1)
  "MCR     p15, 0, r0, c1, c0, 1                   \n"  // Write CP15 Auxiliary Control Register

  // Set Vector Base Address Register (VBAR) to point to this application's vector table
  "LDR    R0, =Vectors                             \n"
  "MCR    p15, 0, R0, c12, c0, 0                   \n"

  // Setup Stack for each exceptional mode
  "CPS    #0x11                                    \n"
  "LDR    SP, =Image$$FIQ_STACK$$ZI$$Limit         \n"
  "CPS    #0x12                                    \n"
  "LDR    SP, =Image$$IRQ_STACK$$ZI$$Limit         \n"
  "CPS    #0x13                                    \n"
  "LDR    SP, =Image$$SVC_STACK$$ZI$$Limit         \n"
  "CPS    #0x17                                    \n"
  "LDR    SP, =Image$$ABT_STACK$$ZI$$Limit         \n"
  "CPS    #0x1B                                    \n"
  "LDR    SP, =Image$$UND_STACK$$ZI$$Limit         \n"
  "CPS    #0x1F                                    \n"
  "LDR    SP, =Image$$SYS_STACK$$ZI$$Limit         \n"
  );

  SystemInit();

  // Zero fill the bss segment
  extern unsigned int __zero_table_start__;
  unsigned int *zeroStart = &__zero_table_start__;
  unsigned long zeroSectionLen = zeroStart[1];
  bss_init(__zero_table_start__, zeroSectionLen);

  // Init Rd/Wr data
  extern unsigned int __copy_table_start__;
  unsigned int *copyStart = &__copy_table_start__;
  unsigned long dstData = copyStart[1];
  unsigned long cpyDataLen = copyStart[2];
  data_init(__copy_table_start__, dstData, cpyDataLen);

  // Call C++ library initialisation
  __libc_init_array();

  // Enable Interrupts
  __ASM volatile("CPSIE if");

  if (pdPASS != xTaskCreate(free_rtos_main,
                            "main",
                            configMAIN_STACK_SIZE,
                            NULL,
                            tskIDLE_PRIORITY + 1,
                            NULL))
  {
    while (1)
      ;
  }

  // Start the real time scheduler.
  vTaskStartScheduler();
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void) {
  while (1);
}

int sys_semihost(int reason, volatile void *arg)
{
  // Exit implements SYS_EXIT_EXTENDED semihosting command.
  // This will exit QEMU session when running program in QEMU.
  // On arm32 this is the only way to return exit code.
  // On arm64 SYS_EXIT is enough.
  // ref. https://github.com/ARM-software/abi-aa/blob/main/semihosting/semihosting.rst

  volatile register int value asm("r0") = reason;
  volatile register void *data asm("r1") = arg;
  asm volatile (
    "  SVC #0x123456  "
    : "=r"(value)
    : "0"(value), "r"(data)
  );

  return value;
}

extern "C" void _exit(int exit_code)
{
  constexpr int SYS_EXIT_EXTENDED = 0x20u;
  constexpr int ADP_Stopped_ApplicationExit = 0x20026u;
  volatile int ret[2] = {ADP_Stopped_ApplicationExit, exit_code};
  sys_semihost(SYS_EXIT_EXTENDED, ret);
}
