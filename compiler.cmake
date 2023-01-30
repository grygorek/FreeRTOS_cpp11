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

if(riscv)
# riscv gcc10
# SET(COMPILER_PREFIX riscv-none-embed)
# riscv gcc11
  SET(COMPILER_PREFIX riscv-none-elf)
else(riscv)
  SET(COMPILER_PREFIX arm-none-eabi)
endif()

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION TRUE)
SET(CMAKE_CROSSCOMPILING TRUE)
SET(CMAKE_USE_RELATIVE_PATHS TRUE)
SET(CMAKE_C_COMPILER   ${COMPILER_PREFIX}-gcc${COMPILER_POSTFIX})
SET(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})
SET(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-g++${COMPILER_POSTFIX})

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

# GCC 7.2.1 includes. Or set env path
#INCLUDE_DIRECTORIES(
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1/arm-none-eabi
#	${COMPILER_PATH}/${COMPILER_PREFIX}/include/c++/7.2.1/backward
#	${COMPILER_PATH}/lib/gcc/arm-none-eabi/7.2.1/include
#	${COMPILER_PATH}/lib/gcc/arm-none-eabi/7.2.1/include-fixed
#	include
#)