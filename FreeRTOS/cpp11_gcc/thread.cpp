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

#include <thread>
#include <system_error>
#include <cerrno>
#include "FreeRTOS.h"

#include "gthr_key_type.h"
#include "freertos_thread_attributes.h"

namespace free_rtos_std
{
  extern Key *s_key;

  const attributes *internal::attributes_lock::_attrib{&internal::attributes_lock::_default};
} // namespace free_rtos_std

namespace std
{

  static void __execute_native_thread_routine(void *__p)
  {
    __gthread_t local{*static_cast<__gthread_t *>(__p)}; // copy

    { // we own the arg now; it must be deleted after run() returns
      thread::_State_ptr __t{static_cast<thread::_State *>(local.arg())};
      local.notify_started(); // copy has been made; tell we are running
      __t->_M_run();
    }

    if (free_rtos_std::s_key)
      free_rtos_std::s_key->CallDestructor(__gthread_t::self().native_task_handle());

    local.notify_joined(); // finished; release joined threads
  }

  thread::_State::~_State() = default;

  void thread::_M_start_thread(_State_ptr state, void (*)())
  {
    const int err = __gthread_create(
        &_M_id._M_thread, __execute_native_thread_routine, state.get());

    if (err)
      __throw_system_error(err);

    state.release();
  }

  void thread::join()
  {
    id invalid;
    if (_M_id._M_thread != invalid._M_thread)
      __gthread_join(_M_id._M_thread, nullptr);
    else
      __throw_system_error(EINVAL);

    // destroy the handle explicitly - next call to join/detach will throw
    _M_id = std::move(invalid);
  }

  void thread::detach()
  {
    id invalid;
    if (_M_id._M_thread != invalid._M_thread)
      __gthread_detach(_M_id._M_thread);
    else
      __throw_system_error(EINVAL);

    // destroy the handle explicitly - next call to join/detach will throw
    _M_id = std::move(invalid);
  }

  // Returns the number of concurrent threads supported by the implementation.
  // The value should be considered only a hint.
  //
  // Return value
  //    Number of concurrent threads supported. If the value is not well defined
  //    or not computable, returns ​0​.
  unsigned int thread::hardware_concurrency() noexcept
  {
    return 0; // not computable
  }

  void this_thread::__sleep_for(chrono::seconds sec, chrono::nanoseconds nsec)
  {
    long ms = nsec.count() / 1'000'000;
    if (sec.count() == 0 && ms == 0 && nsec.count() > 0)
      ms = 1; // round up to 1 ms => if sleep time != 0, sleep at least 1ms

    vTaskDelay(pdMS_TO_TICKS(chrono::milliseconds(sec).count() + ms));
  }

} // namespace std
