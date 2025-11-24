// See LICENSE for license details.

#ifndef __UTIL_H
#define __UTIL_H

extern void setStats(int enable);

#include <stdint.h>

#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }

int verify(int n, const volatile int* test, const int* verify);

int verifyDouble(int n, const volatile double* test, const double* verify);

void __attribute__((noinline)) barrier(int ncores);

uint64_t lfsr(uint64_t x);

uintptr_t insn_len(uintptr_t pc);

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

#define NULL 0

int memcmp(const void *s1, const void *s2, unsigned long n);

int strncmp(const char *s1, const char *s2, unsigned long n);

char *strchr(const char *s, int c);

unsigned long strlen(const char *s);

int tolower(int c);

int toupper(int c);

int putchar(int c);

int puts(const char *string);

void *memcpy(void *dest, const void *src, unsigned long n);

void bcopy(const void *src, void *dest, long unsigned int n);

void *memmove(void *dest, const void *src, unsigned long n);

void *memset(void *ptr, int value, unsigned long num);

void bzero(void *s, unsigned long n);

double modf(double x, double *iptr);

int printf(const char *format, ...);

void *malloc(unsigned long size);

void *calloc(unsigned long len, unsigned long size);

void *realloc(void *ptr, unsigned long size);

void * _sbrk(int increment);

void free(void *ptr);

// math functions and constants
#define PI 3.14159

void srand(unsigned int s);

int rand(void);

double fabs(double x);

double pow(double x, double n);

double sqrt(double x);

double sin(double x);

double cos(double x);

double atan(double z);

double atan2(double y, double x);

double acos(double x);

double cbrt(double x);

double floor(double x);

double ceil(double x);

double ldexp(double x, int exp);

typedef struct {
    double low;
    double high;
} fake_float128;

fake_float128 __extenddftf2(double a);

double __trunctfdf2(fake_float128 a);

fake_float128 __multf3(fake_float128 a, fake_float128 b);

fake_float128 __addtf3(fake_float128 a, fake_float128 b);

fake_float128 __subtf3(fake_float128 a, fake_float128 b);

fake_float128 __divtf3(fake_float128 a, fake_float128 b);

int __lttf2(fake_float128 a, fake_float128 b);

typedef struct _FILE FILE;

int fprintf(FILE *stream, const char *format, ...);

FILE *fopen(const char *pathname, const char *mode);

unsigned long fread(void *ptr, unsigned long size, unsigned long nmemb, FILE *stream);

int fclose(FILE *stream);

#endif //__UTIL_H
