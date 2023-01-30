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

#include <cstddef>
#include <stdint.h>

extern "C"
{
  struct stat;
  int _stat(const char *path, struct stat *buf)
  {
    (void)path;
    (void)buf;
    return -1;
  }
  int _fstat(int fd, struct stat *buf)
  {
    (void)fd;
    (void)buf;
    return -1;
  }

  int _lstat(const char *path, struct stat *buf)
  {
    (void)path;
    (void)buf;
    return -1;
  }

  struct _reent;
  int _open_r(struct _reent *, const char *, int, int)
  {
    return -1;
  }
  int _close(struct _reent *, int)
  {
    return -1;
  }
  long _write(struct _reent *, int, const char *, int)
  {
    return -1;
  }
  long _read(struct _reent *, int, char *, int)
  {
    return -1;
  }

  int _sbrk(int)
  {
    while (1)
      ;
  }
  void _kill(int, int) {}
  int _getpid() { return 1; }
  int _isatty(int) { return -1; }
  int _lseek(int, int, int) { return 0; }

  // FreeRTOS malloc/free declarations
  void *pvPortMalloc(size_t);
  void vPortFree(void *);

  // Redirect malloc to FreeRTOS malloc
  void *__wrap_malloc(size_t size)
  {
    // This is a not aligned version of malloc but still need to match
    // the format expected by free.

    // Allocate one more space to store an address.
    uintptr_t *p = (uintptr_t *)pvPortMalloc(size + sizeof(void *));
    if (!p)
      return nullptr;

    // Store real address that will be used by free().
    ((void **)p)[0] = p;

    return p + 1;
  }

  void *__wrap__malloc_r(void *, size_t size)
  {
    return __wrap_malloc(size);
  }

  // Allocate aligned memory.
  void *__wrap__memalign_r(void *, size_t al, size_t size)
  {
    if (al < sizeof(void *))
      al = sizeof(void *);

    void *p = pvPortMalloc(size + al);
    if (!p)
      return nullptr;

    // Requirement: Alignment 'al' must be power of 2.
    void *aligned_ptr = (void *)(((uintptr_t)p & -al) + al);

    // Store the real address that will be used by free().
    ((void **)aligned_ptr)[-1] = p;

    return aligned_ptr;
  }

  // Redirect free to FreeRTOS free
  void __wrap_free(void *p)
  {
    void *real_ptr = ((void **)p)[-1];
    vPortFree(real_ptr);
  }
}
