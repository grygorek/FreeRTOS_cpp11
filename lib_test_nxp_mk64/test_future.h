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

#ifndef __FUTURE_TEST_H__
#define __FUTURE_TEST_H__

#include <future>
#include <thread>
#include <chrono>
#include <cassert>
#include <cstdint>

inline void TestAsync()
{
  std::future<std::int32_t> result0{
      std::async([]() -> std::int32_t {
        return 2;
      })};

  std::future<std::int32_t> result1{
      std::async(std::launch::async, []() -> std::int32_t {
        return 3;
      })};

  std::future<std::int32_t> result2{
      std::async(std::launch::deferred, []() -> std::int32_t {
        return 5;
      })};

  std::int32_t r{result0.get() + result1.get() + result2.get()};
  assert(2 + 3 + 5 == r);
}

inline void TestSharedFuture()
{
  std::promise<void> go;
  std::shared_future<void> ready{go.get_future()};

  auto fun1{[ready]() -> std::int32_t {
    ready.wait();
    return 5;
  }};

  auto fun2{[ready]() -> std::int32_t {
    ready.wait();
    return 10;
  }};

  auto result1{std::async(std::launch::async, fun1)};
  auto result2{std::async(std::launch::async, fun2)};

  go.set_value();

  std::int32_t r{result1.get() + result2.get()};
  assert(5 + 10 == r);
}

inline void TestSetValueAtExit()
{
  struct Future
  {
    std::promise<std::int32_t> p;
    std::future<std::int32_t> fut{p.get_future()};
  };

  Future f0, f1;

  using namespace std::chrono_literals;
  std::thread{
      [&f0] {
        std::this_thread::sleep_for(2ms);
        f0.p.set_value_at_thread_exit(6);
      }}
      .detach();

  std::thread{
      [&f1] {
        f1.p.set_value_at_thread_exit(7);
      }}
      .detach();

  assert(6 == f0.fut.get() && 7 == f1.fut.get());
}

inline void TestPackagedTask()
{
  std::packaged_task<int(int, int)> task([](int a, int b) {
    return a + b;
  });
  std::future<int> result{task.get_future()};

  task(2, 9);

  assert(11 == result.get());
}

inline void TestFuture()
{
  TestAsync();
  TestSharedFuture();
  TestSetValueAtExit();
  TestPackagedTask();
}

#endif // __FUTURE_TEST_H__
