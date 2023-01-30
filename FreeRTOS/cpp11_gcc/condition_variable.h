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

#ifndef GTHR_FREERTOS_INTERNAL_CV_H
#define GTHR_FREERTOS_INTERNAL_CV_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "thread_gthread.h"

#include <list>
#include <bits/functexcept.h>

namespace free_rtos_std
{

class semaphore
{
public:
  semaphore() : _xSemaphore { xSemaphoreCreateBinary() }
  {
    if (!_xSemaphore)
      std::__throw_system_error(12); // POSIX error: no memory
    xSemaphoreGive(_xSemaphore);
  }

  void lock() { xSemaphoreTake(_xSemaphore, portMAX_DELAY); }
  void unlock() { xSemaphoreGive(_xSemaphore); }

  ~semaphore()
  {
    vSemaphoreDelete(_xSemaphore);
  }

  semaphore(const semaphore &) = delete;
  semaphore(semaphore &&) = delete;
  semaphore &operator=(semaphore &) = delete;
  semaphore &operator=(semaphore &&) = delete;

private:
  SemaphoreHandle_t _xSemaphore;
};

// Internal free rtos task's container to support condition variable.
// Condition variable must know all the threads waiting in a queue.
//
class cv_task_list
{
public:
  using __gthread_t = free_rtos_std::gthr_freertos;
  using thrd_type = __gthread_t::native_task_type;
  using queue_type = std::list<thrd_type>;

  cv_task_list() = default;

  void remove(thrd_type thrd) { _que.remove(thrd); }
  void push(thrd_type thrd) { _que.push_back(thrd); }
  void pop() { _que.pop_front(); }
  bool empty() const { return _que.empty(); }

  ~cv_task_list()
  {
    lock();
    _que = queue_type{};
    unlock();
  }

  // no copy and no move
  cv_task_list &operator=(const cv_task_list &r) = delete;
  cv_task_list &operator=(cv_task_list &&r) = delete;
  cv_task_list(cv_task_list &&) = delete;
  cv_task_list(const cv_task_list &) = delete;

  thrd_type &front() { return _que.front(); }
  const thrd_type &front() const { return _que.front(); }
  thrd_type &back() { return _que.back(); }
  const thrd_type &back() const { return _que.back(); }

  void lock() { _sem.lock(); }
  void unlock() { _sem.unlock(); }

private:
  queue_type _que;
  semaphore _sem;
};
} // namespace free_rtos_std

#endif // GTHR_FREERTOS_INTERNAL_CV_H
