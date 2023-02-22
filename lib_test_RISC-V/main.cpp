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

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <future>
#include <cassert>

#include "test_helpers.h"

#include "console.h"

#include "freertos_time.h"

#include "test_thread.h"
#include "test_cv.h"
#include "test_future.h"
#include "test_once.h"
#include "test_mutex.h"

#if __cplusplus > 201907L
#include "test_semaphore_latch_barrier.h"
#include "test_atomic.h"
#endif

// For updates check my github page:
// https://github.com/grygorek/FreeRTOS_cpp11

int main(void)
{
  using namespace std::chrono_literals;
  using namespace std::chrono;

  print("RISC-V - start test\n");

  SetSystemClockTime(time_point<system_clock>(1550178897s));

  std::this_thread::sleep_until(system_clock::now() + 200ms);

  print("Run...\n");
  for (int i = 0; i < 10; i++)
  {
    print("Iteration - ");
    print(i);
    print("\n");

    TEST_F(TestMtx);

    TEST_F(DetachAfterThreadEnd);
    TEST_F(DetachBeforeThreadEnd);
    TEST_F(JoinAfterThreadEnd);
    TEST_F(JoinBeforeThreadEnd);
    TEST_F(DestroyBeforeThreadEnd);
    TEST_F(DestroyNoStart);
    TEST_F(StartAndMoveOperator);
    TEST_F(StartAndMoveConstructor);
    TEST_F(StartWithStackSize);
    TEST_F(AssignWithStackSize);

#if __cplusplus > 201907L
    TEST_F(TestJThread);

    // Semaphore is not stable in gcc11 (??)
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104928

    TEST_F(TestSemaphore);
    TEST_F(TestLatch);
    TEST_F(TestBarrier);
    TEST_F(TestAtomicWait);
#endif

    TEST_F(TestConditionVariable);
    TEST_F(TestCallOnce);
    TEST_F(TestFuture);
  }
  print("OK\n");
  return EXIT_SUCCESS;
}
