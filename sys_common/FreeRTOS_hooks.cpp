/// Copyright 2021 Piotr Grygorczuk <grygorek@gmail.com>
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