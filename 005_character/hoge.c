#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void redraw();
void printstr(int x, int y, char *str, char color);
void myputchar(int x, int y, char c, char color);

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

void printstr(int x, int y, char *str, char color) {
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    char c = str[i];
    myputchar(x + i * 8, y, c, color);
  }
}

void myputchar(int x, int y, char c, char color) {
  char *hankaku = (char *)&_binary_hankaku_bin_start;
  char *base = (char *)fbRequest.fbBaseAddress;
  char *p;
  char d;

  for (int i = 0; i < 16; i++) {
    p = base + (y + i) * kWidth + x;
    d = hankaku[c * 16 + i];
    if ((d & 0x80) != 0) {
      p[0] = color;
    }
    if ((d & 0x40) != 0) {
      p[1] = color;
    }
    if ((d & 0x20) != 0) {
      p[2] = color;
    }
    if ((d & 0x10) != 0) {
      p[3] = color;
    }
    if ((d & 0x08) != 0) {
      p[4] = color;
    }
    if ((d & 0x04) != 0) {
      p[5] = color;
    }
    if ((d & 0x02) != 0) {
      p[6] = color;
    }
    if ((d & 0x01) != 0) {
      p[7] = color;
    }
  }
}

void redraw() {
  static uint32_t color = 1;
  const int y_offset = 64;

  uint32_t *p = (uint32_t *)fbRequest.fbBaseAddress;
  p += y_offset * kWidth / 4;

  for (int y = y_offset; y < kHeight; y++) {
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

  /* myputchar(0, 0, 'A', 7); */
  printstr(10, 0, "HOG", 7);
  printstr(10 + 8 * 3, 0, "E", 1);
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
