# Copyright 2018-2023 Piotr Grygorczuk <grygorek@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 3.0)

set(frtos_port Source/portable/GCC/${CPU_ARCH})

if(CPU_ARCH STREQUAL "RISC-V")  
  # FreeRTOS port for RISC-v processers have an additional include file
  set(FREERTOS_PORT_INC ${frtos_port}/chip_specific_extensions/RV32I_CLINT_no_extensions)
elseif()
  set(FREERTOS_PORT_INC "")
endif()  

include_directories(
  cpp11_gcc 
  Source/include
  ${frtos_port}
  ${FREERTOS_PORT_INC}
)

if(CPU_ARCH STREQUAL "ARM_CA9" OR CPU_ARCH STREQUAL "RISC-V")  
  # FreeRTOS port for Cortex A and RISC-v processers have an additional asm file
  set(FREERTOS_PORT_ASM ${frtos_port}/portASM.S)
elseif()
  set(FREERTOS_PORT_ASM "")
endif()  

add_library(freeRTOS STATIC
  cpp11_gcc/freertos_time.cpp
  cpp11_gcc/gthr_key.cpp
  cpp11_gcc/thread.cpp

  Source/croutine.c
  Source/event_groups.c
  Source/list.c
  Source/queue.c
  Source/stream_buffer.c
  Source/tasks.c
  Source/timers.c

  Source/portable/MemMang/heap_4.c
  ${frtos_port}/port.c
  ${FREERTOS_PORT_ASM}
)

