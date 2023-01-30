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

#ifndef CORTEX_A9_GLOBAL_TIMER_H_
#define CORTEX_A9_GLOBAL_TIMER_H_

#include "ARMCA9.h"
#include <stdbool.h>

#ifdef __cplus_plus
extern "C" {
#endif

struct GlobTimer_CA9
{
  volatile unsigned long counterLo;
  volatile unsigned long counterHi;
  volatile unsigned long control;
  volatile unsigned long irq_status;
  volatile unsigned long compareLo;
  volatile unsigned long compareHi;
  volatile unsigned long auto_inc;
};

static const unsigned long GLOBTMR_CTRL_EN = 1 << 0;
static const unsigned long GLOBTMR_CTRL_CMP_EN = 1 << 1;
static const unsigned long GLOBTMR_CTRL_IRQ_EN = 1 << 2;
static const unsigned long GLOBTMR_CTRL_AUTO_INC = 1 << 3;
static const unsigned long GLOBTMR_PRESC_BIT = 8;

static const unsigned long GLOBTMR_PEND_EVNT = 1 << 0;

extern struct GlobTimer_CA9 *GLOBTMR;

static inline void GlobTimer_Init(void)
{ // 0x200 is the GT offset (see ARM Cortex-a9 technical specification)
  GLOBTMR = (struct GlobTimer_CA9 *)(__get_CBAR() + 0x200U);
}

static inline void GlobTimer_Compare(bool fEnable)
{
  if (fEnable) 
    GLOBTMR->control |= GLOBTMR_CTRL_CMP_EN;
  else
    GLOBTMR->control &= ~GLOBTMR_CTRL_CMP_EN;
}

static inline void GlobalTimer_IncValue(unsigned long incValue)
  { GLOBTMR->auto_inc = incValue; }

static inline void GlobTimer_CompareValue(unsigned long long cmp)
{
  unsigned long ctrl = GLOBTMR->control;
  unsigned long tmp = ctrl & GLOBTMR_CTRL_CMP_EN;
  GLOBTMR->control = ctrl & ~GLOBTMR_CTRL_CMP_EN;
  GLOBTMR->compareLo = cmp;
  GLOBTMR->compareHi = cmp >> 32;
  GLOBTMR->control |= tmp; // enable only if it was enabled
}

static inline void GlobTimer_CompareValueInc(unsigned long long v)
{
  unsigned long ctrl = GLOBTMR->control;
  unsigned long tmp = ctrl & GLOBTMR_CTRL_CMP_EN;
  GLOBTMR->control = ctrl & ~GLOBTMR_CTRL_CMP_EN;
  unsigned long long c = ((unsigned long long)GLOBTMR->compareHi) << 32;
  c += GLOBTMR->compareLo + v;
  GLOBTMR->compareHi = c >> 32;
  GLOBTMR->compareLo = c;
  GLOBTMR->control |= tmp;
}

static inline void GlobTimer_Enable(bool fEnable)
{
  if (fEnable)
    GLOBTMR->control |= GLOBTMR_CTRL_EN;
  else
    GLOBTMR->control &= ~GLOBTMR_CTRL_EN;
}

static inline void GlobTimer_Prescaller(unsigned int p)
  { GLOBTMR->control |= (0xFFU & p) << GLOBTMR_PRESC_BIT; }

static inline void GlobTimer_AutoInc(bool fEnable)
{
  if (fEnable)
    GLOBTMR->control |= GLOBTMR_CTRL_AUTO_INC;
  else
    GLOBTMR->control &= ~GLOBTMR_CTRL_AUTO_INC;
}

static inline void GlobTimer_Irq(bool fEnable)
{
  if (fEnable)
    GLOBTMR->control |= GLOBTMR_CTRL_IRQ_EN;
  else
    GLOBTMR->control &= ~GLOBTMR_CTRL_IRQ_EN;
}

static inline void GlobTimer_IrqClr(void)
  { GLOBTMR->irq_status |= GLOBTMR_PEND_EVNT; }

static inline bool GlobTimer_EvntPending(void)
  { return 0 != (GLOBTMR->irq_status & GLOBTMR_PEND_EVNT); }

static inline unsigned long long GlobTimer_Counter(void)
{
  for (int i = 3; i--;)
  {
    unsigned long long h = GLOBTMR->counterHi;
    unsigned long l = GLOBTMR->counterLo;
    unsigned long hh = GLOBTMR->counterHi;
    if (h == hh)
      return (h << 32) | l;
  }

  // something is wrong, value is incorrect
  return 0;
}

#ifdef __cplus_plus
}
#endif

#endif //CORTEX_A9_GLOBAL_TIMER_H_
