/// @file
///
/// @author: Piotr Grygorczuk grygorek@gmail.com
///
/// @copyright Copyright 2019 Piotr Grygorczuk
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// o Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
///
/// o Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// o My name may not be used to endorse or promote products derived from this
///   software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#include "FreeRTOS.h"

// Idle task
StaticTask_t g_idleTaskTCB;
StackType_t g_idleTaskStack[96];

// Timer task
StaticTask_t g_timerTaskTCB;
StackType_t g_timerTaskStack[256];

extern "C"
{
  void vApplicationTickHook() {}

  void vApplicationMallocFailedHook()
  {
    while (1)
      ;
  }

  void vApplicationStackOverflowHook()
  {
    while (1)
      ;
  }
#if (configSUPPORT_STATIC_ALLOCATION == 1)
  void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
  {
    *ppxIdleTaskTCBBuffer = &g_idleTaskTCB;
    *ppxIdleTaskStackBuffer = g_idleTaskStack;
    *pulIdleTaskStackSize = 96;
  }

  void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
  {
    *ppxTimerTaskTCBBuffer = &g_timerTaskTCB;
    *ppxTimerTaskStackBuffer = g_timerTaskStack;
    *pulTimerTaskStackSize = 256;
  }
#endif
}