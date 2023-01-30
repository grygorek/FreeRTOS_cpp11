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

#include "gthr_key_type.h"
#include <cassert>

namespace free_rtos_std
{

Key *s_key;

int freertos_gthread_key_create(Key **keyp, void (*dtor)(void *))
{
  // There is only one key for all threads. If more keys are needed
  // a list must be implemented.
  assert(!s_key);
  s_key = new Key(dtor);

  *keyp = s_key;
  return 0;
}

int freertos_gthread_key_delete(Key *)
{
  // no synchronization here:
  //   It is up to the applicaiton to delete (or maintain a reference)
  //   the thread specific data associated with the key.
  delete s_key;
  s_key = nullptr;
  return 0;
}

void *freertos_gthread_getspecific(Key *key)
{
  std::lock_guard<std::mutex> lg{key->_mtx};

  auto item = key->_specValue.find(__gthread_t::self().native_task_handle());
  if (item == key->_specValue.end())
    return nullptr;
  return const_cast<void *>(item->second);
}

int freertos_gthread_setspecific(Key *key, const void *ptr)
{
  std::lock_guard<std::mutex> lg{key->_mtx};

  auto &cont{key->_specValue};
  auto task{__gthread_t::self().native_task_handle()};
  if (ptr)
    cont[task] = ptr;
  else
    (void)cont.erase(task);
  return 0;
}

} // namespace free_rtos_std
