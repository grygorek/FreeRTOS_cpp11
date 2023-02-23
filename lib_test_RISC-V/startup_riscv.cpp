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

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

// Forward declaration
int main();

extern "C"
{

  // Init C++ stuff
  void __libc_init_array();

  void free_rtos_main(void *)
  {
    exit(main());
  }

  void _system_start()
  {
    __libc_init_array();

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

  int sys_semihost(int reason, void *arg)
  {
    // Exit implements SYS_EXIT_EXTENDED semihosting command.
    // This will exit QEMU session when running program in QEMU.
    // ARM has specification here:
    // ref. https://github.com/ARM-software/abi-aa/blob/main/semihosting/semihosting.rst
    //
    // RISC-V is here. Bit short but assuming comaptibile with ARM
    // riscv: https://github.com/riscv-software-src/riscv-semihosting/blob/main/riscv-semihosting-spec.adoc
    register int value asm("a0") = reason;
    register void *data asm("a1") = arg;
    asm volatile(
        " .option push \n"
        " .option norvc \n" /* 32bit instructions */
        " .balign 16 \n"    /* 16-byte alignment, place 3 instructions within the same page. */
        " slli zero, zero, 0x1f \n"
        " ebreak \n"
        " srai zero, zero, 0x7 \n"
        " .option pop \n"
        : "=r"(value)
        : "0"(value), "r"(data)
        : "memory");
    return value;
  }

  void _exit(int exit_code)
  {
    constexpr int SYS_EXIT_EXTENDED = 0x20u;
    constexpr int ADP_Stopped_ApplicationExit = 0x20026u;
    int ret[2] = {ADP_Stopped_ApplicationExit, exit_code};
    sys_semihost(SYS_EXIT_EXTENDED, ret);
  }
}
