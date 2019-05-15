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

#ifndef __CV_TEST_H__
#define __CV_TEST_H__

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>

inline void TestCV()
{
  std::queue<int> q;
  std::mutex m;
  std::condition_variable cv;

  std::this_thread::sleep_for(std::chrono::seconds(1));

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

  std::this_thread::sleep_for(std::chrono::seconds(1));

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

#endif //__CV_TEST_H__
