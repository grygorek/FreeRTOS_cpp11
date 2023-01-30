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
