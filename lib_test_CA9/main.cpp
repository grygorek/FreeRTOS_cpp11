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

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <future>
#include <cassert>

#include "console.h"

#include "freertos_time.h"

#include "test_thread.h"
#include "test_cv.h"
#include "test_future.h"
#include "test_once.h"
#include "test_mutex.h"

// For updates check my github page:
// https://github.com/grygorek/FreeRTOS_cpp11

int main(void)
{
  using namespace std::chrono_literals;
  using namespace std::chrono;

  print("ARM CA9 - start test\n");

  SetSystemClockTime(time_point<system_clock>(1550178897s));


  while (1)
  {
    std::this_thread::sleep_until(system_clock::now() + 200ms);

    print("Run...");

    TestMtx();

    DetachAfterThreadEnd();
    DetachBeforeThreadEnd();
    JoinAfterThreadEnd();
    JoinBeforeThreadEnd();
    DestroyBeforeThreadEnd();
    DestroyNoStart();
    StartAndMoveOperator();
    StartAndMoveConstructor();
#if __cplusplus > 201703L
    TestJThread();
#endif

    TestConditionVariable();

    TestCallOnce();
    TestFuture();

    print("OK\n");
  }
}
