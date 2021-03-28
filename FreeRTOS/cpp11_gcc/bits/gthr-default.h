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

#ifndef _GTHR_FREERTOS_X__H_
#define _GTHR_FREERTOS_X__H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "thread_gthread.h"
#include "condition_variable.h"
#include "gthr_key.h"

#include <sys/time.h>

typedef free_rtos_std::gthr_freertos __gthread_t;

extern "C"
{

#define __GTHREADS 1

  // returns: 1 - thread system is active; 0 - thread system is not active
  static int __gthread_active_p() { return 1; }

  typedef free_rtos_std::Key *__gthread_key_t;
  typedef int __gthread_once_t;
  typedef SemaphoreHandle_t __gthread_mutex_t;
  typedef SemaphoreHandle_t __gthread_recursive_mutex_t;
  typedef free_rtos_std::cv_task_list __gthread_cond_t;

#define __GTHREAD_ONCE_INIT 0

  static inline void __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION(
      __gthread_recursive_mutex_t *mutex)
  {
    *mutex = xSemaphoreCreateRecursiveMutex();
  }
  static inline void __GTHREAD_MUTEX_INIT_FUNCTION(__gthread_mutex_t *mutex)
  {
    *mutex = xSemaphoreCreateMutex();
  }

  static int __gthread_once(__gthread_once_t *once, void (*func)(void))
  {
    static __gthread_mutex_t s_m = xSemaphoreCreateMutex();
    if (!s_m)
      return 12; //POSIX error: ENOMEM

    __gthread_once_t flag{true};
    xSemaphoreTakeRecursive(s_m, portMAX_DELAY);
    std::swap(*once, flag);
    xSemaphoreGiveRecursive(s_m);

    if (flag == false)
      func();

    return 0;
  }

  static int __gthread_key_create(__gthread_key_t *keyp, void (*dtor)(void *))
  {
    return free_rtos_std::freertos_gthread_key_create(keyp, dtor);
  }

  static int __gthread_key_delete(__gthread_key_t key)
  {
    return free_rtos_std::freertos_gthread_key_delete(key);
  }

  static void *__gthread_getspecific(__gthread_key_t key)
  {
    return free_rtos_std::freertos_gthread_getspecific(key);
  }

  static int __gthread_setspecific(__gthread_key_t key, const void *ptr)
  {
    return free_rtos_std::freertos_gthread_setspecific(key, ptr);
  }
  //////////

  //////////
  static inline int __gthread_mutex_destroy(__gthread_mutex_t *mutex)
  {
    vSemaphoreDelete(*mutex);
    return 0;
  }
  static inline int __gthread_recursive_mutex_destroy(
      __gthread_recursive_mutex_t *mutex)
  {
    vSemaphoreDelete(*mutex);
    return 0;
  }

  static inline int __gthread_mutex_lock(__gthread_mutex_t *mutex)
  {
    return (xSemaphoreTake(*mutex, portMAX_DELAY) == pdTRUE) ? 0 : 1;
  }
  static inline int __gthread_mutex_trylock(__gthread_mutex_t *mutex)
  {
    return (xSemaphoreTake(*mutex, 0) == pdTRUE) ? 0 : 1;
  }
  static inline int __gthread_mutex_unlock(__gthread_mutex_t *mutex)
  {
    return (xSemaphoreGive(*mutex) == pdTRUE) ? 0 : 1;
  }

  static inline int __gthread_recursive_mutex_lock(
      __gthread_recursive_mutex_t *mutex)
  {
    return (xSemaphoreTakeRecursive(*mutex, portMAX_DELAY) == pdTRUE) ? 0 : 1;
  }
  static inline int __gthread_recursive_mutex_trylock(
      __gthread_recursive_mutex_t *mutex)
  {
    return (xSemaphoreTakeRecursive(*mutex, 0) == pdTRUE) ? 0 : 1;
  }
  static inline int __gthread_recursive_mutex_unlock(
      __gthread_recursive_mutex_t *mutex)
  {
    return (xSemaphoreGiveRecursive(*mutex) == pdTRUE) ? 0 : 1;
  }
////////////

////////////
#define __GTHREADS_CXX0X 1

  struct __gthread_time_t
  {
    long sec;
    long nsec;
    int64_t milliseconds() const
    {
      return int64_t(sec) * 1000 + nsec / 1'000'000;
    }
  };

  static inline __gthread_time_t operator-(
      const __gthread_time_t &lhs, const timeval &rhs)
  {
    int32_t s{lhs.sec - rhs.tv_sec};
    int32_t ns{lhs.nsec - rhs.tv_usec * 1000};

    return __gthread_time_t{s, ns};
  }

  static inline int __gthread_mutex_timedlock(
      __gthread_mutex_t *m, const __gthread_time_t *abs_timeout)
  {
    timeval now{};
    gettimeofday(&now, NULL);

    auto t = (*abs_timeout - now).milliseconds();
    return (xSemaphoreTake(*m, pdMS_TO_TICKS(t)) == pdTRUE) ? 0 : 1;
  }

  static inline int __gthread_recursive_mutex_timedlock(
      __gthread_recursive_mutex_t *m, const __gthread_time_t *abs_time)
  {
    timeval now{};
    gettimeofday(&now, NULL);

    auto t = (*abs_time - now).milliseconds();
    return (xSemaphoreTakeRecursive(*m, pdMS_TO_TICKS(t)) == pdTRUE) ? 0 : 1;
  }

  // All functions returning int should return zero on success or the error
  //    number.  If the operation is not supported, -1 is returned.

  static inline int __gthread_create(__gthread_t *thread, void (*func)(void *),
                                     void *args)
  {
    return thread->create_thread(func, args) ? 0 : 1;
  }
  static inline int __gthread_join(__gthread_t &thread, void **value_ptr)
  {
    thread.join();
    return 0;
  }
  static inline int __gthread_detach(__gthread_t &thread)
  {
    thread.detach();
    return 0;
  }
  static inline int __gthread_equal(const __gthread_t &t1,
                                    const __gthread_t &t2)
  {
    return t1 == t2 ? 0 : 1;
  }
  static inline __gthread_t __gthread_self(void) { return __gthread_t::self(); }

#define _GLIBCXX_USE_SCHED_YIELD

  static inline int __gthread_yield(void)
  {
    taskYIELD();
    return 0;
  }

  //      not used - condition_variable has its own 'notify' function
  //static inline int __gthread_cond_signal(__gthread_cond_t* cond) {
  //  return -1;
  //}

  static inline int __gthread_cond_timedwait(
      __gthread_cond_t *cond, __gthread_mutex_t *mutex,
      const __gthread_time_t *abs_timeout)
  {
    auto this_thrd_hndl{__gthread_t::native_task_handle()};
    cond->lock();
    cond->push(this_thrd_hndl);
    cond->unlock();

    timeval now{};
    gettimeofday(&now, NULL);

    auto ms{(*abs_timeout - now).milliseconds()};

    __gthread_mutex_unlock(mutex);
    auto fTimeout{0 == ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(ms))};
    __gthread_mutex_lock(mutex);

    int result{0};
    if (fTimeout)
    { // timeout - remove the thread from the waiting list
      cond->lock();
      cond->remove(this_thrd_hndl);
      cond->unlock();
      result = 138; // posix ETIMEDOUT
    }

    return result;
  }

} // extern "C"

#endif // _GTHR_FREERTOS_X__H_
