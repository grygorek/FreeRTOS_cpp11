# @file
#
# @author: Piotr Grygorczuk grygorek@gmail.com
#
# @copyright Copyright 2019 Piotr Grygorczuk
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# o Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# o Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# o My name may not be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required(VERSION 3.0)

# ARM Cortex-M3 settings
###########################
message(STATUS "Building: LM3S811 board")
set(CPU_ARCH "ARM_CM3")
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")

file(GLOB LINKER_SCRIPTS  "${CMAKE_SOURCE_DIR}/qemu_lm3s811/*.ld")
file(COPY ${LINKER_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR}) 

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--wrap=malloc -Wl,--wrap=free  -Wl,--gc-sections")
set(LINKER_SCRIPT "linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -Xlinker -Map=output.map")

###################
# Project settings
###################

#      Build Settings
#------------------------
SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -Os")
if(DEBUG)
  SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -g")
endif(DEBUG)

# lto is broken in gcc 8.2
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
SET(COMPILE_COMMON_FLAGS "${COMPILE_PART_FLAGS} -Wall -Wextra -Wpedantic -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections")

# When enabling exceptions, change the linker script to link libstdc++.a instead of libstdc++_nano.a
# Note: It is expect LM3S811 has got not enough memory to build with full libstdc++.
SET(CMAKE_C_FLAGS   "${COMPILE_COMMON_FLAGS} -std=c11 -nostdlib -ffreestanding -fno-builtin " CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${COMPILE_COMMON_FLAGS} -std=c++1z -nostdlib -ffreestanding -fno-builtin -fno-exceptions -fno-rtti -fno-unwind-tables" CACHE INTERNAL "" FORCE)
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${COMPILE_PART_FLAGS}"  CACHE INTERNAL "" FORCE)

# definition required by the demo project
add_definitions(-DGCC_ARMCM3_LM3S102 -Dgcc)

include_directories(
  qemu_lm3s811
  qemu_lm3s811/hw_include
  qemu_lm3s811/freertos_demo/Common/include

  FreeRTOS/Source/include 
  FreeRTOS
  FreeRTOS/Source/portable/${COMPILER_NAME}/${CPU_ARCH}
  FreeRTOS/cpp11_gcc
)

add_subdirectory(FreeRTOS)

add_executable(${PROJECT_NAME}.elf  
  sys_common/FreeRTOS_hooks.cpp
  sys_common/FreeRTOS_memory.cpp
  sys_common/sys.cpp
  
  qemu_lm3s811/startup.cpp
  qemu_lm3s811/main.cpp
  qemu_lm3s811/hw_include/src/gpio.c
  qemu_lm3s811/hw_include/src/i2c.c
  qemu_lm3s811/hw_include/src/interrupt.c
  qemu_lm3s811/hw_include/src/uart.c
  qemu_lm3s811/hw_include/src/osram96x16.c

  qemu_lm3s811/freertos_Demo/Common/Minimal/integer.c
  qemu_lm3s811/freertos_Demo/Common/Minimal/BlockQ.c
  qemu_lm3s811/freertos_Demo/Common/Minimal/PollQ.c
  qemu_lm3s811/freertos_Demo/Common/Minimal/semtest.c
)

find_library(driver libdriver.a PATHS ${CMAKE_SOURCE_DIR}/qemu_lm3s811/hw_include REQUIRED)

target_link_libraries(
  ${PROJECT_NAME}.elf
  freeRTOS
  ${driver}
)

# Generate binary 'bin' from elf
add_custom_command(
  OUTPUT binary
  POST_BUILD COMMAND ${COMPILER_PREFIX}-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin 
  DEPENDS ${PROJECT_NAME}.elf 
  COMMENT "Generate 'bin'"
)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS binary)

