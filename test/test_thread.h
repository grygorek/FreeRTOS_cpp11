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

#ifndef THREAD_TEST_H
#define THREAD_TEST_H

#include <thread>
#include <chrono>
#include <stop_token>
#include <numeric>

#include "thread_with_attributes.h"

inline void DetachBeforeThreadEnd()
{
  using namespace std::chrono_literals;
  std::thread t{[] {
    std::this_thread::sleep_for(50ms);
  }};

  t.detach();
}

inline void DetachAfterThreadEnd()
{
  using namespace std::chrono_literals;
  std::thread t{[] {
  }};

  std::this_thread::sleep_for(50ms);
  t.detach();
}

inline void JoinBeforeThreadEnd()
{
  using namespace std::chrono_literals;
  std::thread t{[] {
    std::this_thread::sleep_for(50ms);
  }};

  t.join();
}

inline void JoinAfterThreadEnd()
{
  using namespace std::chrono_literals;
  std::thread t{[] {
  }};

  std::this_thread::sleep_for(50ms);
  t.join();
}

inline void DestroyBeforeThreadEnd()
{
  //using namespace std::chrono_literals;
  // will call std::terminate if enabled
  //	std::thread t{[]{
  //			std::this_thread::sleep_for(50ms);
  //	}};
}

inline void DestroyNoStart()
{
  std::thread t;
}

inline void StartAndMoveOperator()
{
  using namespace std::chrono_literals;
  std::thread tt;

  {
    std::thread t{[] {
      std::this_thread::sleep_for(50ms);
    }};
    tt = std::move(t);
  }

  tt.join();
}

inline void StartAndMoveConstructor()
{
  using namespace std::chrono_literals;

  std::thread t{[] {
    std::this_thread::sleep_for(50ms);
  }};

  std::thread tt{std::move(t)};

  //t.join(); this will terminate the program
  tt.join();
}

inline void StartWithStackSize()
{
  using namespace std::chrono_literals;

  const auto fn = []{
    constexpr size_t ARR_SIZE{3072U}; // 3 kB
    std::array<uint8_t, ARR_SIZE> arr;
    std::iota(std::begin(arr), std::end(arr), 0U);
    std::this_thread::sleep_for(50ms);
  };

  using namespace free_rtos_std;
  // Create a thread with a custom stack size.
  // Stack would overflow without this
  std::thread t = std_thread(attr_stack_size(1024U), fn);
  t.join();
}

inline void AssignWithStackSize()
{
  using namespace std::chrono_literals;

  const auto fn = []{
    constexpr size_t ARR_SIZE{3072U}; // 3 kB
    std::array<uint8_t, ARR_SIZE> arr;
    std::iota(std::begin(arr), std::end(arr), 0U);
    std::this_thread::sleep_for(50ms);
  };

  using namespace free_rtos_std;
  std::thread t;
  // Assign a thread with a custom stack size.
  // Stack would overflow without this
  t = std_thread(attr_stack_size(1024U),fn);
  t.join();
}

#if __cplusplus > 201703L
inline void TestJThread()
{
  using namespace std::chrono_literals;

  std::jthread t{ [](std::stop_token stop){
    while( !stop.stop_requested() )
      std::this_thread::sleep_for(1ms);
  }};

  std::this_thread::sleep_for(20ms);
  t.request_stop();
}
#endif

#endif //__THREAD_TEST_H__
