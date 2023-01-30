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

#ifndef __MTX_TEST_H__
#define __MTX_TEST_H__

#include <chrono>
#include <mutex>

#include <cassert>

inline void TestRecursiveMtx()
{
  std::recursive_mutex to_mtx;

  // expected the mutex is available
  to_mtx.lock();

  // expected the mutex is available for the same thread
  assert(to_mtx.try_lock() == true);

  //call twice according to the ownership level
  to_mtx.unlock();
  to_mtx.unlock();
}

inline void TestTimedMtx()
{
  using namespace std::chrono_literals;
  using namespace std::chrono;

  std::timed_mutex to_mtx;

  // expected the mutex is available
  to_mtx.lock();

  // expected the mutex is not available
  assert(to_mtx.try_lock() == false);
  assert(to_mtx.try_lock_for(10ms) == false);
  assert(to_mtx.try_lock_until(system_clock::now() + 200ms) == false);

  to_mtx.unlock();

  assert(to_mtx.try_lock() == true);
  to_mtx.unlock();

  assert(to_mtx.try_lock_for(10ms) == true);
  to_mtx.unlock();

  assert(to_mtx.try_lock_until(system_clock::now() + 200ms) == true);
  to_mtx.unlock();
}

inline void TestMtx()
{
  TestRecursiveMtx();
  TestTimedMtx();
}

#endif //__MTX_TEST_H__
