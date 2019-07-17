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

extern "C"
{
  void _exit()
  {
    while (1)
      ;
  }

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
  void* pvPortMalloc(unsigned long long);
  void vPortFree(void*);

  // Redirect malloc to FreeRTOS malloc
  void* __wrap_malloc(unsigned long long size)
  {
    return pvPortMalloc(size);
  }

  // Redirect free to FreeRTOS free
  void __wrap_free(void *p)
  {
    vPortFree(p);
  }
}
