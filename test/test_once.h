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

#ifndef __CALL_ONCE_TEST_H__
#define __CALL_ONCE_TEST_H__

#include <mutex>
#include <thread>
#include <cassert>

inline void TestCallOnce()
{
  auto cnt{0};
  std::once_flag f;
  std::mutex mtx;

  auto once{
      [&] {
        std::call_once(f, [&cnt, &mtx] {
          // If call_once works this guard is not needed.
          // If it does not work, make sure cnt is incremented atomicaly.
          std::lock_guard<std::mutex> lg{mtx};
          cnt++;
        });
      }};

  std::thread t0{once};
  std::thread t1{once};
  std::thread t2{once};
  std::thread t3{once};

  t0.join();
  t1.join();
  t2.join();
  t3.join();

  assert(cnt == 1);
}

#endif // __CALL_ONCE_TEST_H__
