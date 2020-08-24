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

#ifndef __MTX_TEST_H__
#define __MTX_TEST_H__

#include <chrono>
#include <mutex>

#include <cassert>

inline void TestRecursiveMtx()
{
  std::recursive_mutex to_mtx;

  // expected the mutex is available
  to_mtx.lock();

  // expected the mutex is available for the same thread
  assert(to_mtx.try_lock() == true);

  //call twice according to the ownership level
  to_mtx.unlock();
  to_mtx.unlock();
}

inline void TestTimedMtx()
{
  using namespace std::chrono_literals;
  using namespace std::chrono;

  std::timed_mutex to_mtx;

  // expected the mutex is available
  to_mtx.lock();

  // expected the mutex is not available
  assert(to_mtx.try_lock() == false);
  assert(to_mtx.try_lock_for(10ms) == false);
  assert(to_mtx.try_lock_until(system_clock::now() + 200ms) == false);

  to_mtx.unlock();

  assert(to_mtx.try_lock() == true);
  to_mtx.unlock();

  assert(to_mtx.try_lock_for(10ms) == true);
  to_mtx.unlock();

  assert(to_mtx.try_lock_until(system_clock::now() + 200ms) == true);
  to_mtx.unlock();
}

inline void TestMtx()
{
  TestRecursiveMtx();
  TestTimedMtx();
}

#endif //__MTX_TEST_H__
