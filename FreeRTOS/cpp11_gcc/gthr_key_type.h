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

#ifndef __FREERTOS_GTHR_KEY_KEY_H__
#define __FREERTOS_GTHR_KEY_KEY_H__

#include "thread_gthread.h"
#include <unordered_map>
#include <mutex>

namespace free_rtos_std
{

struct Key
{
  using __gthread_t = free_rtos_std::gthr_freertos;
  typedef void (*DestructorFoo)(void *);

  Key() = delete;
  explicit Key(DestructorFoo des) : _desFoo{des} {}

  void CallDestructor(__gthread_t::native_task_type task)
  {
    void *val;

    {
      std::lock_guard<std::mutex> lg{_mtx};

      auto item{_specValue.find(task)};
      if (item == _specValue.end())
        return;

      val = const_cast<void *>(item->second);
      _specValue.erase(item);
    }

    if (_desFoo && val)
      _desFoo(val);
  }

  std::mutex _mtx;
  DestructorFoo _desFoo;
  std::unordered_map<__gthread_t::native_task_type, const void *> _specValue;
};

} // namespace free_rtos_std

#endif //__FREERTOS_GTHR_KEY_KEY_H__
