/// @file
///
/// @author: Piotr Grygorczuk grygorek@gmail.com
///
/// @copyright Copyright 2019 Piotr Grygorczuk
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// o Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
///
/// o Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// o My name may not be used to endorse or promote products derived from this
///   software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <future>
#include <cassert>

#include "FreeRTOS_time.h"

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
  SetSystemClockTime(time_point<system_clock>(1550178897s));

  while (1)
  {
    std::this_thread::sleep_until(system_clock::now() + 200ms);

    TestMtx();

    DetachAfterThreadEnd();
    DetachBeforeThreadEnd();
    JoinAfterThreadEnd();
    JoinBeforeThreadEnd();
    DestroyBeforeThreadEnd();
    DestroyNoStart();
    StartAndMoveOperator();
    StartAndMoveConstructor();

    TestConditionVariable();

    TestCallOnce();
    TestFuture();
  }
}

/* System clock frequency. */
extern "C" uint32_t SystemCoreClockFreq()
{
  return 20'000'000;
}
