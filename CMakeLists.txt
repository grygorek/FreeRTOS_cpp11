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


include(compiler.cmake)

if(k64frdmevk)
  project(lib_test_nxp_mk64 C CXX ASM)
  include(lib_test_nxp_mk64.cmake)
endif()

# Print output section size
add_custom_command(
  OUTPUT print_size
  POST_BUILD COMMAND ${COMPILER_PREFIX}-size ${PROJECT_NAME}.elf 
  DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf
  COMMENT "Binary size"
)

add_custom_target(size ALL DEPENDS print_size) 
