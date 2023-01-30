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

# ARM Cortex-A9 settings
###########################
message(STATUS "Building: ARM vexpress-a9")
set(CPU_ARCH "ARM_CA9")
set(CPU_FLAGS "-mcpu=cortex-a9 -mfloat-abi=hard -mfpu=auto")

set(APPLICATION_DIR "lib_test_CA9")

file(GLOB LINKER_SCRIPTS  "${CMAKE_SOURCE_DIR}/${APPLICATION_DIR}/*.ld")
file(COPY ${LINKER_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR}) 

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=aligned_alloc -Wl,--wrap=_malloc_r -Wl,--wrap=_memalign_r -Wl,--gc-sections")
set(LINKER_SCRIPT "linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

###################
# Project settings
###################

#      Build Settings
#------------------------
SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -Os")
if(DEBUG)
  SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -g3")
endif(DEBUG)

# lto is broken in gcc 8.2
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
SET(COMPILE_COMMON_FLAGS "${CONFIG_DEFS} ${COMPILE_PART_FLAGS} -Wall -Wextra -Wpedantic -fno-common  \
                        -fmessage-length=0 -ffunction-sections -fdata-sections")

# When enabling exceptions, change the linker script to link libstdc++.a instead of libstdc++_nano.a
SET(CMAKE_C_FLAGS   "${COMPILE_COMMON_FLAGS} -std=c17  -nostdlib -ffreestanding -fno-builtin " CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${COMPILE_COMMON_FLAGS} -std=c++2a -nostdlib -ffreestanding -fno-builtin -fno-exceptions -fno-rtti -fno-unwind-tables" CACHE INTERNAL "" FORCE)
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${COMPILE_PART_FLAGS}"  CACHE INTERNAL "" FORCE)

include_directories( 
  ${APPLICATION_DIR}
  ${APPLICATION_DIR}/cmsis
  
  test

  FreeRTOS/Source/include 
  FreeRTOS 
  FreeRTOS/Source/portable/${COMPILER_NAME}/${CPU_ARCH}
  FreeRTOS/cpp11_gcc
)

# Select the right directory for the compiler version
if(CMAKE_CXX_COMPILER_VERSION LESS 11)
  SET(GCC_VER_DIR "v10")
else(CMAKE_CXX_COMPILER_VERSION LESS 11)
  SET(GCC_VER_DIR "v11")
endif(CMAKE_CXX_COMPILER_VERSION LESS 11)

add_subdirectory(FreeRTOS)
add_subdirectory(libstdc++_gcc/${GCC_VER_DIR})

add_executable(${PROJECT_NAME}.elf  
  ${APPLICATION_DIR}/startup_ARMCA9.cpp
  ${APPLICATION_DIR}/system_ARMCA9.c
  ${APPLICATION_DIR}/console.cpp
  ${APPLICATION_DIR}/ca9_global_timer.c
  ${APPLICATION_DIR}/cmsis/irq_ctrl_gic.c
  ${APPLICATION_DIR}/FreeRTOS_ca9_hooks.c
  ${APPLICATION_DIR}/main.cpp

  sys_common/FreeRTOS_hooks.cpp
  sys_common/FreeRTOS_memory.cpp
  sys_common/sys.cpp
)

target_link_libraries(
  ${PROJECT_NAME}.elf
  freeRTOS
  std++_freertos
)
