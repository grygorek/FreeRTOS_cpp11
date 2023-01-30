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

#ifndef __CV_TEST_H__
#define __CV_TEST_H__

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <array>

#include <cassert>

inline void TestCVTimeout()
{
  // Idea of this test is to force condition variable to timeout.
  //
  // It is achieved by having shorter wait timeout than new data is added
  // to the processing queue. There is number of processing threads
  // concuring for data. They have different timeouts to shuffle order
  // of condition variable waiting threads queue.

  std::queue<int> q;
  std::mutex m;
  std::condition_variable cv;
  bool fCancel{false};

  { // test generic timeout
    std::unique_lock<std::mutex> lock{m};
    assert(false == cv.wait_for(lock, std::chrono::milliseconds(10), [&q, &fCancel] { return q.size() > 0 || fCancel; }));
  }

  auto processFoo{[&](std::int32_t idx) {
    std::unique_lock<std::mutex> lock{m};

    while (1)
    {
      while (false == cv.wait_for(lock, std::chrono::milliseconds(idx * 2),
                                  [&q, &fCancel] { return q.size() > 0 || fCancel; }))
        ;

      if (q.size())
      {
        q.pop();
        lock.unlock();
      }
      else
      {
        lock.unlock();
        return;
      }

      lock.lock();
    }
  }};

  std::array<std::thread, 6> processors;
  std::int32_t size{processors.size()};
  for (auto &processor : processors)
    processor = std::thread{processFoo, size--};

  for (auto i{20}; i >= 0; i--)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    m.lock();
    q.push(i);
    m.unlock();
    cv.notify_one();
  }

  fCancel = true;

  for (auto &processor : processors)
    processor.join();

  assert(q.size() == 0);
}

inline void TestCV()
{
  std::queue<int> q;
  std::mutex m;
  std::condition_variable cv;

  std::thread processor{[&]() {
    std::unique_lock<std::mutex> lock{m};

    while (1)
    {
      cv.wait(lock, [&q] { return q.size() > 0; });
      int i = q.front();
      q.pop();
      lock.unlock();

      if (i == 0)
        return;

      lock.lock();
    }
  }};

  for (int i = 100; i >= 0; i--)
  {
    m.lock();
    q.push(i);
    m.unlock();
    cv.notify_one();
  }

  processor.join();
}

inline void TestCVAny()
{
  std::queue<int> q;
  std::mutex m;
  std::condition_variable_any cv;

  std::thread processor{[&]() {
    m.lock();

    while (1)
    {
      cv.wait(m, [&q] { return q.size() > 0; });

      int i = q.front();
      q.pop();
      m.unlock();

      if (i == 0)
        return;

      m.lock();
    }
  }};

  for (int i = 100; i >= 0; i--)
  {
    m.lock();
    q.push(i);
    m.unlock();
    cv.notify_one();
  }

  processor.join();
}

inline void TestNotifyAllAtThrdExit()
{
  std::condition_variable cv;
  std::mutex mtx;
  std::int32_t result{};
  bool fReady{false};

  std::thread{
      [&] {
        std::unique_lock<std::mutex> lg{mtx};
        result = 666;
        fReady = true;
        std::notify_all_at_thread_exit(cv, std::move(lg));
      }}
      .detach();

  std::unique_lock<std::mutex> lg{mtx};
  cv.wait(lg, [&fReady] { return fReady; });

  assert(666 == result);
}

inline void TestConditionVariable()
{
  TestCV();
  TestCVAny();
  TestCVTimeout();
  TestNotifyAllAtThrdExit();
}

#endif //__CV_TEST_H__
