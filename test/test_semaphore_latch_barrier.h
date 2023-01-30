/// Copyright 2022 Piotr Grygorczuk <grygorek@gmail.com>
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

#ifndef SEMAPHORE_LATCH_BARRIER_TEST_H__
#define SEMAPHORE_LATCH_BARRIER_TEST_H__

#include <semaphore>
#include <latch>
#include <barrier>
#include <thread>
#include <array>
#include <vector>
#include <cassert>
#include <string>

inline void TestSemaphore()
{
  int test{};
  std::binary_semaphore sem(0);
  std::jthread t{[&sem, &test]
                 {
                   test = 5;
                   sem.release();
                 }};

  sem.acquire();

  assert(5 == test);
}

inline void TestLatch()
{
  std::array<int, 5> test;
  std::latch work_done(test.size());

  for (auto &t : test)
  {
    std::thread{[&t, &work_done]
                {
                  t = 1;
                  work_done.count_down();
                }}
        .detach();
  }

  work_done.wait();

  for (auto &t : test)
    assert(t == 1);
}

inline void TestBarrier()
{
  std::array<int, 3> test{};
  int cnt = 0;

  std::barrier sync{test.size(), [&]
                    {
                      if (cnt == 0)
                        for (auto &i : test)
                          assert(i == 1);

                      if (cnt == 1)
                        for (auto &i : test)
                          assert(i == 2);

                      ++cnt;
                    }};

  auto work = [&](int &i)
  {
    i = 1;
    sync.arrive_and_wait();
    i = 2;
    sync.arrive_and_wait();
  };

  std::vector<std::jthread> workers;
  for (auto &i : test)
    workers.emplace_back(work, std::ref(i));

  for (auto &w : workers)
    w.join();

  assert(cnt == 2);
}

#endif // SEMAPHORE_LATCH_BARRIER_TEST_H__
