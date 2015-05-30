#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void redraw();

volatile static bool g_interrupt = false;
volatile static uint32_t g_frames = 0;

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

  FillRect(0, 16, kWidth, kHeight - 16, color);
  color = ++color % 8;

  FillRect(3, 20, 64, 40, 1);
  FillRect(233, 210, 128, 20, 2);
  FillRect(403, 400, 96, 50, 3);

  g_frames++;
}

void draw_fps() {
  char line[256];

  // clear the 1st line
  FillRect(0, 0, kWidth, 16, 0);

  sprintf(line, "%d FPS", g_frames);
  PrintStr(0, 0, "*", 1);
  PrintStr(16, 0, line, 7);
}

int main(int argc, char const *argv[]) {
  rpiInit();
  FramebufferInitialize();

  _init_vector_table();

  // disable IRQ
  *INTERRUPT_DISABLE_BASIC_IRQS = 0xffffffff;
  *INTERRUPT_DISABLE_IRQS1 = 0xffffffff;
  *INTERRUPT_DISABLE_IRQS2 = 0xffffffff;
  *INTERRUPT_FIQ_CTRL = 0;

  _disable_IRQ();

  // set Timer Interrupt
  *INTERRUPT_ENABLE_BASIC_IRQS = 0x01;

  // stop timer
  *TIMER_CONTROL &= 0xffffff00;

  // set timer clock -> 1MHz
  //（0xF9=249: timer clock=250MHz/(249+1)）
  *TIMER_PREDIVIDER = 0x000000F9;

  // Timer (1000 msec)
  *TIMER_LOAD = 1000000 - 1;
  *TIMER_RELOAD = 1000000 - 1;

  // clear IRQ flag
  *TIMER_IRQ_CLR = 0;

  // start timer
  *TIMER_CONTROL |= 0x000000A2;

  // enable interrupts
  *INTERRUPT_ENABLE_BASIC_IRQS = 0x01;

  // enable IRQ
  _enable_IRQ();

  while (true) {
    redraw();

    _disable_IRQ();
    if (true == g_interrupt) {
      g_interrupt = false;
      _enable_IRQ();
      draw_fps();
      g_frames = 0;
    } else {
      _enable_IRQ();
    }
  }
  return 0;
}
