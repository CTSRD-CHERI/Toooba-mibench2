
#include <stdint.h>
#include "uart_16550.h"
#if __CHERI__
#include <cheric.h>
#endif

// This is a null implementation for simulation which does not print,
// but also does not crash.

int uart0_init(void)
{
  uint32_t divisor;
  divisor = UART_CLOCK_RATE / (16 * DEFAULT_BAUDRATE);

#if __CHERI__
  pio = cheri_setoffset(__builtin_cheri_global_data_get(), (size_t) pio);
  pio = cheri_csetbounds((void *) pio, sizeof(struct uart_pio));
#endif

  return 0;
}


int uart0_rxready(void)
{
  return 0; //((pio->lsr & LSR_DR) != 0);
}


int uart0_rxchar(void)
{
  //while ((pio->lsr & LSR_DR) == 0)
  //  ;  /* Wait */

  return 0; //pio->rbr;
}

int uart0_txbuffer(char *ptr, int len) {
  //int cnt;
  //for (cnt = 0; cnt < len; cnt++) {
  //  uart0_txchar (*ptr++);
  //}
  return len;
}

int uart0_txchar(int c)
{
  //while ((pio->lsr & LSR_THRE) == 0)
  //  ;  /* Wait */

  //pio->thr = c;

  return 0;//c;
}

/* Wait for transmitter shift register/FIFO to empty */
void uart0_flush(void)
{
  //while ((pio->lsr & LSR_TEMT) == 0)
  //  ;  /* Wait */

  //while ((pio->lsr & LSR_THRE) == 0)
  //  ;  /* Wait */
}
