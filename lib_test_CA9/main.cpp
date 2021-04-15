#include <thread>
#include <chrono>

volatile unsigned int *const UART0DR = (unsigned int *)0x10009000; // ???

static inline void printc(char c) { *UART0DR = c; }

void print(const char *s)
{
  while (*s != '\0')
  {
    *UART0DR = (unsigned int)(*s);
    s++;
  }
}

void print(unsigned int num)
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

const char array[] = "Hello World 1!\n";

extern "C" void main(void *)
{
  print("Hello world!\n");
  print((unsigned)array);
  printc('\n');

  for (;;)
  {
    print("Hello world!\n");
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
  }
}
