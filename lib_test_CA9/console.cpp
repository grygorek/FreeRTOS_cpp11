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

#include "console.h"

// UART data register
volatile unsigned int *const UART0DR = (unsigned int *)0x10009000; // ???
volatile unsigned int *const UART0FR = (unsigned int *)0x10009006; // ???

static void wait_for_ready()
{
  while (*UART0FR & (1 << 7))
    ;
}

void print(const char *s)
{
  while (*s != '\0')
  { // missing: waiting for the data register to be empty
    wait_for_ready();
    *UART0DR = (unsigned int)(*s);
    s++;
  }
}

void print(unsigned int num)
{
  static const char tab[] = "0123456789ABCDEF";
  for (int i = 8; i >= 0; i--)
  { // missing: waiting for the data register to be empty
    unsigned char c = (num >> (i * 4)) & 0xF;
    wait_for_ready();
    if (c < sizeof(tab) - 1)
      *UART0DR = tab[c];
    else
      *UART0DR = '?';
  }
}
