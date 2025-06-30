// See LICENSE for license details.

#ifndef __UTIL_H
#define __UTIL_H

extern void setStats(int enable);

#include <stdint.h>

#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }

static int verify(int n, const volatile int* test, const int* verify)
{
  int i;
  // Unrolled for faster verification
  for (i = 0; i < n/2*2; i+=2)
  {
    int t0 = test[i], t1 = test[i+1];
    int v0 = verify[i], v1 = verify[i+1];
    if (t0 != v0) return i+1;
    if (t1 != v1) return i+2;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static int verifyDouble(int n, const volatile double* test, const double* verify)
{
  int i;
  // Unrolled for faster verification
  for (i = 0; i < n/2*2; i+=2)
  {
    double t0 = test[i], t1 = test[i+1];
    double v0 = verify[i], v1 = verify[i+1];
    int eq1 = t0 == v0, eq2 = t1 == v1;
    if (!(eq1 & eq2)) return i+1+eq1;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static void __attribute__((noinline)) barrier(int ncores)
{
  static volatile int sense;
  static volatile int count;
  static __thread int threadsense;

  __sync_synchronize();

  threadsense = !threadsense;
  if (__sync_fetch_and_add(&count, 1) == ncores-1)
  {
    count = 0;
    sense = threadsense;
  }
  else while(sense != threadsense)
    ;

  __sync_synchronize();
}

static uint64_t lfsr(uint64_t x)
{
  uint64_t bit = (x ^ (x >> 1)) & 1;
  return (x >> 1) | (bit << 62);
}

static uintptr_t insn_len(uintptr_t pc)
{
  return (*(unsigned short*)pc & 3) ? 4 : 2;
}

#ifdef __riscv
#include "encoding.h"
#endif

#define stringify_1(s) #s
#define stringify(s) stringify_1(s)
#define stats(code, iter) do { \
    unsigned long _c = -read_csr(mcycle), _i = -read_csr(minstret); \
    code; \
    _c += read_csr(mcycle), _i += read_csr(minstret); \
    if (cid == 0) \
      printf("\n%s: %ld cycles, %ld.%ld cycles/iter, %ld.%ld CPI\n", \
             stringify(code), _c, _c/iter, 10*_c/iter%10, _c/_i, 10*_c/_i%10); \
  } while(0)

int memcmp(const void *s1, const void *s2, int n) {
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;

    for (int i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return (int)(p1[i] - p2[i]);
        }
    }
    return 0;
}

void *memcpy(void *dest, const void *src, int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (int i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

void *memset(void *s, int c, int n) {
    unsigned char *p = s;
    unsigned char value = (unsigned char)c;

    for (int i = 0; i < n; i++) {
        p[i] = value;
    }

    return s;
}

double modf(double x, double *iptr) {
    if (x >= 0.0) {
        *iptr = (double)(long long)x;
    } else {
        *iptr = (double)(long long)x;
    }

    return x - *iptr;
}

int printf(const char *format, ...) {
    // Stub: does nothing
    (void)format;  // Avoid unused parameter warning

    va_list args;
    va_start(args, format);
    va_end(args);

    return 0;  // Return value can be adjusted as needed
}

#define HEAP_SIZE (1024 * 1024)  // 1 MB

static unsigned char heap[HEAP_SIZE];
static int heap_offset = 0;

void *malloc(int size) {
    // Align size to 16 bytes
    size = (size + 15) & ~15;

    if (heap_offset + size > HEAP_SIZE) {
        // Out of memory
        return 0;
    }

    void *ptr = &heap[heap_offset];
    heap_offset += size;
    return ptr;
}

void free(void *ptr) {
    // Stub: does nothing
    (void)ptr; // Avoid unused parameter warning
}

static unsigned int seed = 1;

void srand(unsigned int s) {
    seed = s;
}

int rand(void) {
    // Constants from Numerical Recipes
    seed = seed * 1664525 + 1013904223;
    return (int)(seed & 0x7FFFFFFF);  // Return non-negative result
}

static double power(double x, int n) {
    double result = 1.0;
    for (int i = 0; i < n; i++) {
        result *= x;
    }
    return result;
}

double sin(double x) {
    // Taylor series approximation around 0:
    // sin(x) ≈ x - x^3/3! + x^5/5! - x^7/7!
    double x3 = power(x, 3);
    double x5 = power(x, 5);
    double x7 = power(x, 7);

    return x - x3 / 6.0 + x5 / 120.0 - x7 / 5040.0;
}

double cos(double x) {
    // Taylor series approximation around 0:
    // cos(x) ≈ 1 - x^2/2! + x^4/4! - x^6/6!
    double x2 = power(x, 2);
    double x4 = power(x, 4);
    double x6 = power(x, 6);

    return 1.0 - x2 / 2.0 + x4 / 24.0 - x6 / 720.0;
}

#endif //__UTIL_H
