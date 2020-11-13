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

# ARM Cortex-M4F settings
###########################
message(STATUS "Building: K64FRDM EVK board")
set(CPU_ARCH "ARM_CM4F")
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
 
file(GLOB LINKER_SCRIPTS  "${CMAKE_SOURCE_DIR}/lib_test_nxp_mk64/*.ld")
file(COPY ${LINKER_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR}) 

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--wrap=malloc -Wl,--wrap=free")
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
SET(COMPILE_COMMON_FLAGS "${CONFIG_DEFS} ${COMPILE_PART_FLAGS} -Wall -Wextra -Wpedantic -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections")

# When enabling exceptions, change the linker script to link libstdc++.a instead of libstdc++_nano.a
SET(CMAKE_C_FLAGS   "${COMPILE_COMMON_FLAGS} -std=c11 -nostdlib -ffreestanding -fno-builtin " CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${COMPILE_COMMON_FLAGS} -std=c++1z -nostdlib -ffreestanding -fno-builtin -fno-exceptions -fno-rtti -fno-unwind-tables" CACHE INTERNAL "" FORCE)
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${COMPILE_PART_FLAGS}"  CACHE INTERNAL "" FORCE)

include_directories( 
  lib_test_nxp_mk64

  FreeRTOS/Source/include 
  FreeRTOS 
  FreeRTOS/Source/portable/${COMPILER_NAME}/${CPU_ARCH}
  FreeRTOS/cpp11_gcc
)

add_subdirectory(FreeRTOS)

add_executable(${PROJECT_NAME}.elf  
  lib_test_nxp_mk64/startup_mk64f12.cpp
  lib_test_nxp_mk64/main.cpp

  sys_common/FreeRTOS_hooks.cpp
  sys_common/FreeRTOS_memory.cpp
  sys_common/sys.cpp
)

target_link_libraries(
  ${PROJECT_NAME}.elf
  freeRTOS
)
