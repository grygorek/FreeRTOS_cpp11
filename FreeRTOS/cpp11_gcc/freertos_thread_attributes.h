/// Copyright 2018-2023 Piotr Grygorczuk <grygorek@gmail.com>
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

#ifndef FREERTOS_THREAD_ATTRIBUTES_H__
#define FREERTOS_THREAD_ATTRIBUTES_H__

#include "FreeRTOS.h"

namespace free_rtos_std
{

  struct attributes
  {
    // Name length limit is defined by configMAX_TASK_NAME_LEN
    const char *taskName = "Task";

#ifdef configDEFAULT_STD_THREAD_STACK_SIZE
    configSTACK_DEPTH_TYPE stackWordCount{configDEFAULT_STD_THREAD_STACK_SIZE};
#else
    // Default stack size is 512 words, so 2 kB
    configSTACK_DEPTH_TYPE stackWordCount{512U};
#endif

    UBaseType_t priority = tskIDLE_PRIORITY + 1;
  };

  constexpr attributes attr_stack_size(configSTACK_DEPTH_TYPE size) { return {.stackWordCount = size}; }
  constexpr attributes attr_priority(UBaseType_t priority) { return {.priority = priority}; }
  constexpr attributes attr_name(const char *n) { return {.taskName = n}; }

}

#endif // FREERTOS_THREAD_ATTRIBUTES_H__
