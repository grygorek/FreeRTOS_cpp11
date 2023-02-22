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

#ifndef TEST_HELPERS_H__
#define TEST_HELPERS_H__

#include <string>
#include "console.h"

template <typename F>
void tst_call(const char *name, F fun)
{
  using namespace std::string_literals;
  print(("Running ---- "s + name + " -----\n").c_str());
  fun();
  print("Done\n\n");
}

#define TEST_F(function_) tst_call(#function_, function_)

template <typename T, typename U>
void tst_equal(const char *func, const char *file, int line, T exp, U rcv)
{
  using namespace std::string_literals;
  if (exp == rcv)
  {
    auto pass = "\tPASS - "s + func + ":" + std::to_string(line) + "\n"s;
    print(pass.c_str());
  }
  else
  {
    auto fail = "  FAILED - "s + func + "\n\t in file - ";
    fail += file + ":"s + std::to_string(line) + "\n\t\texpected(" + std::to_string(exp) + ") != received(" + std::to_string(rcv) + ")" + "\n"s;
    print(fail.c_str());
  }
}

#define TEST_EQ(expected_, received_) tst_equal(__func__, __FILE__, __LINE__, expected_, received_)

static inline void tst_assert(const char *func, const char *file, int line, bool cond)
{
  using namespace std::string_literals;
  if (cond)
  {
    auto pass = "\tPASS - "s + func + ":" + std::to_string(line) + "\n"s;
    print(pass.c_str());
  }
  else
  {
    auto fail = "  FAILED assertion "s + func + "\n\t in file - ";
    fail += file + ":"s + std::to_string(line) + "\n"s;
    print(fail.c_str());
  }
}

#define TEST_ASSERT(cond_) tst_assert(__func__, __FILE__, __LINE__, cond_)

#endif // TEST_HELPERS_H__
