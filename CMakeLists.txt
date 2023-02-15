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

include(compiler.cmake)

# enable threads
set(CONFIG_DEFS -D_GLIBCXX_HAS_GTHREADS=1)

if(k64frdmevk)
  project(lib_test_nxp_mk64 C CXX ASM)
  include(lib_test_nxp_mk64.cmake)
elseif(lm3s811)
  project(qemu_lm3s811 C CXX ASM)
  include(qemu_lm3s811.cmake)
  message(STATUS  " ") 
  message(STATUS  "Run this demo in QEMU:") 
  message(STATUS  "      qemu-system-arm -M lm3s811evb -kernel qemu_lm3s811.elf")
  message(STATUS  " ") 
elseif(armca9)
  project(test_ca9 C CXX ASM)
  include(vexpress-ca9.cmake)
  message(STATUS  " ")
  message(STATUS  "Run this demo in QEMU:") 
  message(STATUS  "      qemu-system-arm -M vexpress-a9 -m 128M -nographic -kernel test_ca9.elf")
  message(STATUS  " ")
elseif(riscv)
  project(test_riscv C CXX ASM)
  include(riscv-virt.cmake)
  message(STATUS  " ")
  message(STATUS  "Run this demo in QEMU:") 
  message(STATUS  "      qemu-system-riscv32.exe -M virt -nographic -bios none -kernel test.elf")
  message(STATUS  " ")
else(k64frdmevk)
  message(STATUS " ")
  message(STATUS "  Target is missing, Add -D<target>=1, where `target` is:")
  message(STATUS "     k64frdmevk  - NXP K64FRDM board")
  message(STATUS "     lm3s811     - TI board supported by qemu")
  message(STATUS "     armca9      - Cortex-A9, vexpress-ca9 board test for qemu")
  message(STATUS "     riscv       - RISC-V, virt board test for qemu")
  message(STATUS " ")
  message(FATAL_ERROR "Error: Target not specified")
endif()

# Print output section size
add_custom_command(
  OUTPUT print_size
  POST_BUILD COMMAND ${COMPILER_PREFIX}-size ${PROJECT_NAME}.elf 
  DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf
  COMMENT "Binary size"
)

add_custom_target(size ALL DEPENDS print_size) 
