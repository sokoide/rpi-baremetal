#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void redraw();

volatile static bool g_interrupt = false;

// called by _IRQ_interrupt in startup.s
void IRQ_handler(void) {
  // disable IRQ
  _disable_IRQ();

  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    // Timer interrupt handler
    /* timerIRQ_handler(); */
    g_interrupt = true;
    // clear interrupt flag
    *TIMER_IRQ_CLR = 0;
  }

  // enable IRQ
  _enable_IRQ();
}

void redraw() {
  static uint32_t color = 1;

  uint32_t *p = (uint32_t *)fbRequest.fbBaseAddress;
  for (int y = 0; y < kHeight; y++) {
    for (int x = 0; x < kWidth; x += 4 * 8) {
      int32_t c = color << 24 | color << 16 | color << 8 | color;
      *p++ = c;
      *p++ = c;
      *p++ = c;
      *p++ = c;
      *p++ = c;
      *p++ = c;
      *p++ = c;
      *p++ = c;
    }
  }
  color = ++color % 8;
}

int main(int argc, char const *argv[]) {
  rpiInit();
  FramebufferInitialize();

  _init_vector_table();

  // disable IRQ
  *INTERRUPT_DISABLE_BASIC_IRQS = 0xffffffff;
  *INTERRUPT_DISABLE_IRQS1 = 0xffffffff;
  *INTERRUPT_DISABLE_IRQS2 = 0xffffffff;
  *INTERRUPT_FIQ_CTRL = 0;  // 20c

  _disable_IRQ();

  // set Timer Interrupt
  *INTERRUPT_ENABLE_BASIC_IRQS = 0x01;

  // stop timer
  *TIMER_CONTROL &= 0xffffff00;

  // set timer clock -> 1MHz
  //（0xF9=249: timer clock=250MHz/(249+1)）
  *TIMER_PREDIVIDER = 0x000000F9;

  // Timer (100 msec)
  *TIMER_LOAD = 100000 - 1;
  *TIMER_RELOAD = 100000 - 1;

  // clear IRQ flag
  *TIMER_IRQ_CLR = 0;

  // start timer
  *TIMER_CONTROL |= 0x000000A2;

  // enable interrupts
  *INTERRUPT_ENABLE_BASIC_IRQS = 0x01;

  // enable IRQ
  _enable_IRQ();

  while (true) {
    _disable_IRQ();
    if (true == g_interrupt) {
      g_interrupt = false;
      _enable_IRQ();
      redraw();
    } else {
      _enable_IRQ();
    }
  }
  return 0;
}
