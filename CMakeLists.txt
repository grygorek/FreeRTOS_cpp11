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
if(k64frdmevk)
  message(STATUS "Building: K64FRDM EVK board")
  set(TARGET_BOARD "k64FRDMevk" CACHE INTERNAL "" FORCE)
  set(TARGET_CPU "mk64f12")
  set(CPU_ARCH "ARM_CM4F")
  set(CPU_VENDOR "NXP")
  #SET(CONFIG_DEFS "-DCPU_MK64FN1M0VDC12 -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DARMCM4_FP -DENET_ENHANCEDBUFFERDESCRIPTOR_MODE")
  SET(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
else(k64frdmevk)
  message(FATAL_ERROR "Error: Target not specified")
endif(k64frdmevk)
 
# GCC ARM compiler settings
##############################
set(COMPILER_NAME "GCC")
if(WIN32)
  set(COMPILER_PATH "") # add to system path 
  SET(COMPILER_POSTFIX ".exe")
endif(WIN32)
if(LINUX)
  SET(COMPILER_PATH "")
  SET(COMPILER_POSTFIX "")
endif(LINUX)
SET(COMPILER_PREFIX arm-none-eabi)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION TRUE)
SET(CMAKE_CROSSCOMPILING TRUE)
SET(CMAKE_USE_RELATIVE_PATHS TRUE)
#SET(CMAKE_C_COMPILER ${COMPILER_PATH}/bin/${COMPILER_PREFIX}-gcc${COMPILER_POSTFIX})
#SET(CMAKE_CXX_COMPILER ${COMPILER_PATH}/bin/${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})
#SET(CMAKE_ASM_COMPILER ${COMPILER_PATH}/bin/${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})
SET(CMAKE_C_COMPILER   ${COMPILER_PREFIX}-gcc${COMPILER_POSTFIX})
SET(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})
SET(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)



# GCC 7.2.1 includes needed. Or set env path
#INCLUDE_DIRECTORIES(
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1/arm-none-eabi
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1/backward
#	${COMPILER_PATH}/lib/gcc/arm-none-eabi/7.2.1/include
#	${COMPILER_PATH}/lib/gcc/arm-none-eabi/7.2.1/include-fixed
#	include
#)

# Enable C++11 multithreading with FreeRTOS
add_compile_definitions(_GCC_MULTITHREAD_FREERTOS_ENABLE)

file(GLOB LINKER_SCRIPTS  "${CMAKE_SOURCE_DIR}/*.ld")
file(COPY ${LINKER_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR}) 

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--wrap=malloc -Wl,--wrap=free")
set(LINKER_SCRIPT "Linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

###################
# Project settings
###################

project(rtos C CXX ASM)

#      Build Settings
#------------------------
SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -Os")
if(DEBUG)
  SET(COMPILE_PART_FLAGS  "${CPU_FLAGS} -g3")
endif(DEBUG)

SET(COMPILE_COMMON_FLAGS "${CONFIG_DEFS} ${COMPILE_PART_FLAGS} -Wall -Wextra -Wpedantic -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections")
 
SET(CMAKE_C_FLAGS   "${COMPILE_COMMON_FLAGS} -std=c11 -nostdlib -ffreestanding -fno-builtin " CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS "${COMPILE_COMMON_FLAGS} -std=c++1z -nostdlib -ffreestanding -fno-builtin -fno-exceptions -fno-rtti -fno-unwind-tables" CACHE INTERNAL "" FORCE)
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${COMPILE_PART_FLAGS}"  CACHE INTERNAL "" FORCE)

#     FreeRTOS library  
#----------------------------
include_directories( 
  FreeRTOS/Source/include 
  FreeRTOS 
  FreeRTOS/Source/portable/${COMPILER_NAME}/${CPU_ARCH}
  FreeRTOS/cpp11_gcc
)
add_subdirectory(FreeRTOS)
#-----------------------------

add_executable(testapp.elf  
  main.cpp
  sys.cpp
  FreeRTOS_hooks.cpp
  FreeRTOS_memory.cpp
  startup_mk64f12.cpp
)

target_link_libraries(
  testapp.elf
  freeRTOS
)


