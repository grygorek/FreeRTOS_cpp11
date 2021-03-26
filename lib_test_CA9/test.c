#include "FreeRTOS.h"
#include "ca9_global_timer.h"

#include "ARMCA9.h"
#include "irq_ctrl.h"
#include <stdbool.h>

volatile unsigned int *const UART0DR = (unsigned int *)0x10009000; // ???

static inline void print_char(char c) { *UART0DR = c; }

void print_str(const char *s)
{
  while (*s != '\0')
  {
    *UART0DR = (unsigned int)(*s);
    s++;
  }
}

void print_hex(unsigned long long num)
{
  static const char tab[] = "0123456789ABCDEF";
  for (int i = 15; i >= 0; i--)
  {
    unsigned char c = (num >> (i * 4)) & 0xF;
    if (c < sizeof(tab) - 1)
      *UART0DR = tab[c];
    else
      *UART0DR = '?';
  }
}

// clang-format off
#define print(X) _Generic((X),                 \
                          char: print_char,    \
                        char *: print_str,     \
                       default: print_hex)(X)
// clang-format on

// Use global timer
//======================
void main()
{
  char nl = '\n';

  print("Hello world!\n");
  print(__get_CBAR());
  print(nl);

  print(GLOBTMR->control);
  print(nl);

  for (;;)
  {
    print("Hello world!\n");
    vTaskDelay(1000);
  }
}
