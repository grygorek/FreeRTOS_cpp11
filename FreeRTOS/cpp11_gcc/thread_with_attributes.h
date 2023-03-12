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

#ifndef FREERTOS_THREAD_WITH_ATTRIBUTES_H__
#define FREERTOS_THREAD_WITH_ATTRIBUTES_H__

#include <thread>
#include <utility> // std::forward

#include "freertos_thread_attributes.h"

namespace free_rtos_std
{

  // Helper functions to create std::thread and std::jthread with attributes.
  // See free_rtos_std::attributes in freertos_thread_attributes.h for available attributes.
  //
  // Example:
  // ```
  // void thr_f(int i) { std::cout << i; }
  //
  // void f()
  // {
  //   // Create a jtread with 4kB stack size
  //   std::jthread t = free_rtos_std::std_jthread(
  //     free_rtos_std::attr_stack_size(1024u), thr_f, 5);
  // }
  // ```

  // @param args - see arguments of std::thread
  template <typename... Args>
  std::thread std_thread(const free_rtos_std::attributes &attr, Args &&...args)
  {
    free_rtos_std::internal::attributes_lock lock{attr};
    return std::thread(std::forward<Args>(args)...);
  }

  // @param args - see arguments of std::jthread
  template <typename... Args>
  std::jthread std_jthread(const free_rtos_std::attributes &attr, Args &&...args)
  {
    free_rtos_std::internal::attributes_lock lock{attr};
    return std::jthread(std::forward<Args>(args)...);
  }

}

#endif // FREERTOS_THREAD_WITH_ATTRIBUTES_H__
