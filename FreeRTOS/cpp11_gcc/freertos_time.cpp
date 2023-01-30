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

#include "freertos_time.h"
#include "critical_section.h"
#include <sys/time.h>
#include <chrono>
#include "FreeRTOS.h"
#include "task.h"

namespace free_rtos_std
{

class wall_clock
{
public:
  struct time_data
  {
    timeval offset;
    TickType_t ticks;
  };

  static time_data time()
  { //atomic
    critical_section critical;
    return time_data{_timeOffset, xTaskGetTickCount()};
  }

  static void time(const timeval &time)
  { //atomic
    critical_section critical;
    _timeOffset = time;
  }

private:
  static timeval _timeOffset;
};

timeval wall_clock::_timeOffset;

} // namespace free_rtos_std

using namespace std::chrono;
void SetSystemClockTime(
    const time_point<system_clock, system_clock::duration> &time)
{
  auto delta{time - time_point<system_clock>(
                        milliseconds(pdTICKS_TO_MS(xTaskGetTickCount())))};
  long long sec{duration_cast<seconds>(delta).count()};
  long usec = duration_cast<microseconds>(delta).count() - sec * 1'000'000; //narrowing type

  free_rtos_std::wall_clock::time({sec, usec});
}

static timeval operator+(const timeval &l, const timeval &r)
{
  timeval t{l.tv_sec + r.tv_sec, l.tv_usec + r.tv_usec};

  if (t.tv_usec >= 1'000'000)
  {
    t.tv_sec++;
    t.tv_usec -= 1'000'000;
  }
  return t;
}

extern "C" int _gettimeofday(timeval *tv, void *tzvp)
{
  (void)tzvp;

  auto t{free_rtos_std::wall_clock::time()};

  long long ms{pdTICKS_TO_MS(t.ticks)};
  long long sec{ms / 1000};
  long usec = (ms - sec * 1000) * 1000; //narrowing type

  *tv = t.offset + timeval{sec, usec};

  return 0; // return non-zero for error
}
