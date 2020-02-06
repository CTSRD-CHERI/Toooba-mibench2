// See LICENSE for license details.

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <sys/signal.h>
#include "util.h"

#if __CHERI__
#include <cheri_init_globals.h>

int main(int, char * __capability * __capability);

void
_start_purecap(void) {
  cheri_init_globals_3(__builtin_cheri_global_data_get(),
      __builtin_cheri_program_counter_get(),
      __builtin_cheri_global_data_get());
}
#endif

void handle_trap() {
  while(1);
}

void _init(int cid, int nc)
{
  // only single-threaded programs should ever get here.
  int ret = main(0, 0);

  while(1);
}
