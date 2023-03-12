/// Copyright 2018-2023 Piotr Grygorczuk <grygorek@gmail.com>
/// Copyright 2023 by NXP. All rights reserved.
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

#ifndef GTHR_FREERTOS_INTERNAL_H
#define GTHR_FREERTOS_INTERNAL_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "critical_section.h"
#include "freertos_thread_attributes.h"

#include <utility>   // std::forward
#include <exception> // std::terminate

#if __cplusplus > 201703L
#include <compare>
#endif

namespace std
{
  class thread;
}

namespace free_rtos_std
{
  namespace internal
  {
    // Derive from the cticical_section. As long as an instance of attributes_lock exists
    // it is safe to create a thread with custom attributes.
    struct attributes_lock : critical_section
    {
      // Note - attributes_lock should not be used by the end user. Helper API is
      // provided in 'thread_with_attributes.h'. That header should be included by the
      // end user when using custom attributes is required.
      //
      // It is possible to manually specify the task attributes when creating a
      // `std::thread` instance as follows:
      // ```
      // std::thread t{[&] {
      //     free_rtos_std::internal::attributes_lock lock{{.stackWordCount = 4096U}}; // 16 kB
      //     return std::thread{fn, args};
      // }()};
      // ```
      // This way, we are sure that only this one thread will have the specified
      // attributes, while keeping the convenient `std::thread` API.
      //
      // Please note that the following will result in a deadlock:
      // ```
      // std::thread t;
      // {
      //   free_rtos_std::internal::attributes_lock lock{{.stackWordCount = 4096U}}; // 16 kB
      //   t = std::thread{fn, args};
      // }
      // ```
      // The reason is that the move assignment operator of std::thread calls the
      // `gthr_freertos::wait_for_start` method, which waits for a different
      // task. Since scheduling is disabled during the lifetime of
      // `attributes_lock`, that waiting will never return. Instead, if
      // the `std::thread` object only needs to be assigned, we can proceed as
      // follows:
      // ```
      // t = [&] {
      //   free_rtos_std::internal::attributes_lock lock{{.stackWordCount = 4096U}}; // 16 kB
      //   return std::thread{fn, args};
      // }();
      // ```
      // In this case, the move assignment operator is called only after the
      // `attributes_lock` instance is destroyed, so the scheduler is
      // running again, but the thread has already been created.
      //
      attributes_lock(const attributes &attrib) { _attrib = &attrib; }
      ~attributes_lock() { _attrib = &_default; }

      static const attributes *_attrib;

    private:
      static constexpr attributes _default{};
    };
  }

  class gthr_freertos
  {
    // 1. std::thread class has a single member variable representing
    //    a native thread handle
    // 2. Detach requires that the native thread function will execute
    //    even if the std::thread instance has been destroyed. The native
    //    thread function must take the ownership of any resources allocated
    //    during the thread creation. This could be the thread handle itself.
    //    Detach must make sure that the thread has started execution and
    //    the ownership has been taken.
    // 3. Join requires a way to switch the current context to a waiting
    //    state. The native thread function must have a way to unlock
    //    a joined thread.
    // 4. FreeRTOS does not have an interface implementing join. It is possible
    //    to suspend a thread but the thread we are waiting for to join is not
    //    aware which thread is waiting.  Is this statement wrong?
    // 5. Solution is to use events group.
    //    The handle life time must be handled by this class.
    // 6. Life time of thread handle and events group handle is not the same.
    //    There are two cases:
    //     a) detach is called and std::thread instance is destroyed; in this case
    //        thread function outlives the thread instance
    //     b) thread function exits first; in this case the thread instance
    //        outlives the thread function; join must have access to the events
    //        group to check whether the thread function has finished or not.
    //    So, the thread handle must exist as long as thread function executes.
    //    The events group must live as long as std::thread instance exists.
    // 7. Although the living time is different for two handles,
    //    see pt. 1 -only one member variable to handle free rtos interface.
    //    For this reason a single class is a container for the two handles.

    friend std::thread;

    enum
    {
      eEvStoragePos = 0,
      eStartedEv = 1 << 22,
      eJoinEv = 1 << 23
    };

  public:
    typedef void (*task_foo)(void *);
    typedef TaskHandle_t native_task_type;

    gthr_freertos(const gthr_freertos &r)
    {
      critical_section critical;

      _taskHandle = r._taskHandle;
      _evHandle = r._evHandle;
      _arg = r._arg;
      _fOwner = false; // it is just a copy
    }

    gthr_freertos(gthr_freertos &&r)
    {
      taskENTER_CRITICAL();
      if (r._fOwner)
      {
        taskEXIT_CRITICAL();
        // make sure it has already started
        // - native thread function must make local copies first
        r.wait_for_start();
        taskENTER_CRITICAL();
      }
      // 'this' becomes the owner if r is the owner
      move(std::forward<gthr_freertos>(r));
      taskEXIT_CRITICAL();
    }

    bool create_thread(task_foo foo, void *arg)
    {
      _arg = arg;

      _evHandle = xEventGroupCreate();
      if (!_evHandle)
        std::terminate();

      {
        critical_section critical;

        const auto &attr = *internal::attributes_lock::_attrib;
        xTaskCreate(foo, attr.taskName, attr.stackWordCount, this, attr.priority, &_taskHandle);
        if (!_taskHandle)
          std::terminate();

        vTaskSetThreadLocalStoragePointer(_taskHandle, eEvStoragePos, _evHandle);
        _fOwner = true;
      }

      return true;
    }

    void join()
    { // note 1: _evHandle must be valid here. Even if the native thread function
      //   has finished and got destroyed the _taskHandle, it
      //   must not release the _evHandle!
      //
      // note 2: The native thread function must call notify_joined when it has
      //   finished. Even if 'join' is called after that, the bits are set
      //   and event group will not block. So it is safe to call wait here.
      //
      // note 3: Instead of keeping _evHandle, it could be possible to read
      //   it from the local storage of the task. However, that would require
      //   checking if the task still exists and use critical section if it does.
      //   It is faster to use _evHandle directly, even if it is an extra item to
      //   copy each time when this instance is copied.
      while (0 == xEventGroupWaitBits(_evHandle,
                                      eJoinEv | eStartedEv,
                                      pdFALSE,
                                      pdTRUE,
                                      portMAX_DELAY))
        ;
    }

    void detach()
    { // Detaching is removing the event's object. It can be done
      // only if the thread has started execution.
      wait_for_start();

      { // unfortunately critical section is needed here to make sure
        // the task is not deleted while accessing the task's local storage.
        critical_section critical;

        if (eDeleted != eTaskGetState(_taskHandle))
        {
          vTaskSetThreadLocalStoragePointer(_taskHandle, eEvStoragePos, nullptr);
          vEventGroupDelete(_evHandle);

          // Thread still exists but detach removes ownership.
          // Ownership belongs to the native thread. It will release the task
          // handle once user's thread function exits.
          _fOwner = false;
        }
      }
    }

    void notify_started()
    { // Function should be called only from the controlled task
      // and only when the (internal, not application's one) thread function
      // has started execution.
      xEventGroupSetBits(_evHandle, eStartedEv);
    }

    void notify_joined()
    { // Function should be called only from the controlled task
      // and only when the thread function has finished execution.
      {
        critical_section critical;

        // Note: The _evHandle may be invalid if thread has been detached.
        //    It is required to get the handle from the thread itself.
        //    The detach function will set this handle to nullptr.
        auto evHnd = static_cast<EventGroupHandle_t>(
            pvTaskGetThreadLocalStoragePointer(_taskHandle, eEvStoragePos));

        if (evHnd)
        {
          xEventGroupSetBits(evHnd, eJoinEv);
        }
        else
        {
          // thread has been detached, nothing to do
        }
      }

      // vTaskDelete will not return
      vTaskDelete(nullptr);
    }

    static gthr_freertos self()
    {
      auto tHnd = xTaskGetCurrentTaskHandle();
      auto evHnd = static_cast<EventGroupHandle_t>(
          pvTaskGetThreadLocalStoragePointer(tHnd, eEvStoragePos));
      return gthr_freertos{tHnd, evHnd};
    }

    static native_task_type native_task_handle()
    {
      return xTaskGetCurrentTaskHandle();
    }

    constexpr friend bool operator==(const gthr_freertos &l, const gthr_freertos &r) noexcept
    {
      return l._taskHandle == r._taskHandle;
    }
#if __cpp_lib_three_way_comparison
    constexpr friend std::strong_ordering operator<=>(const gthr_freertos &l, const gthr_freertos &r) noexcept
    {
      return l._taskHandle <=> r._taskHandle;
    }
#else
    constexpr friend bool operator!=(const gthr_freertos &l, const gthr_freertos &r) noexcept
    {
      return l._taskHandle != r._taskHandle;
    }
    constexpr friend bool operator<(const gthr_freertos &l, const gthr_freertos &r) noexcept
    {
      return l._taskHandle < r._taskHandle;
    }
#endif

    constexpr void *arg() const
    {
      return _arg;
    }

    ~gthr_freertos() = default;
    gthr_freertos &operator=(const gthr_freertos &r) = delete;

    gthr_freertos() = default;

    constexpr gthr_freertos(native_task_type thnd, EventGroupHandle_t ehnd)
        : _taskHandle{thnd}, _evHandle{ehnd}
    {
    }

    gthr_freertos &operator=(gthr_freertos &&r)
    {
      if (this == &r)
        return *this;

      taskENTER_CRITICAL();

      if (_fOwner)
      { // If 'r' is the owner then 'this' will get the
        // new ownership. If 'r' is not the owner then
        // just a copy is being moved. Either way 'this'
        // ownership is lost and handles must be deleted.
        if (eDeleted != eTaskGetState(_taskHandle))
          vTaskDelete(_taskHandle);
        if (_evHandle)
          vEventGroupDelete(_evHandle);
        _fOwner = false;
      }
      else if (r._fOwner)
      {
        taskEXIT_CRITICAL();
        // make sure it has already started
        // - native thread function must make local copies first
        r.wait_for_start();
        taskENTER_CRITICAL();
      }
      // 'this' becomes the owner if r is the owner
      move(std::forward<gthr_freertos>(r));
      taskEXIT_CRITICAL();
      return *this;
    }

  private:
    constexpr void move(gthr_freertos &&r)
    {
      _taskHandle = r._taskHandle;
      _evHandle = r._evHandle;
      _arg = r._arg;
      _fOwner = r._fOwner;
      r._taskHandle = nullptr;
      r._evHandle = nullptr;
      r._arg = nullptr;
      r._fOwner = false;
    }

    void wait_for_start()
    {
      while (0 == xEventGroupWaitBits(
                      _evHandle, eStartedEv, pdFALSE, pdTRUE, portMAX_DELAY))
        ;
    }

    native_task_type _taskHandle{nullptr};
    EventGroupHandle_t _evHandle{nullptr};
    void *_arg{nullptr};
    bool _fOwner{false};
  };

} // namespace free_rtos_std

#endif // GTHR_FREERTOS_INTERNAL_H
