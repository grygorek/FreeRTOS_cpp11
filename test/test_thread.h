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

#ifndef __THREAD_TEST_H__
#define __THREAD_TEST_H__

#include <thread>
#include <chrono>

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

#endif //__THREAD_TEST_H__
