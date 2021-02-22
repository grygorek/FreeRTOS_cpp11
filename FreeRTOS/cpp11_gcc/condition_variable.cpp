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

#include <condition_variable>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

namespace std
{

  condition_variable::condition_variable() = default;

  condition_variable::~condition_variable()
  { // It is only safe to invoke the destructor if all threads have been notified.
    if (!_M_cond.empty())
      std::__throw_system_error(117); // POSIX error: structure needs cleaning
  }

  void condition_variable::wait(std::unique_lock<std::mutex> &m)
  { // pre-condition: m is taken!!
    _M_cond.lock();
    _M_cond.push(__gthread_t::native_task_handle());
    _M_cond.unlock();

    m.unlock();

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    m.lock();
  }

  void condition_variable::notify_one()
  {
    _M_cond.lock();
    if (!_M_cond.empty())
    {
      auto t = _M_cond.front();
      _M_cond.pop();
      xTaskNotifyGive(t);
    }
    _M_cond.unlock();
  }

  void condition_variable::notify_all()
  {
    _M_cond.lock();
    while (!_M_cond.empty())
    {
      auto t = _M_cond.front();
      _M_cond.pop();
      xTaskNotifyGive(t);
    }
    _M_cond.unlock();
  }
} // namespace std