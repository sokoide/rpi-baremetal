#include "rpi.h"

TIMERCTL timerctl;

void InitTimer(FIFO8* fifo) {
  // init timerctl;
  timerctl.count = 0;
  timerctl.fifo = fifo;
  for (int i = 0; i < MAX_TIMER; i++) {
    timerctl.timer[i].timeout = 0;
  }

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

  // Timer (1 msec)
  *TIMER_LOAD = 1000 - 1;
  *TIMER_RELOAD = 1000 - 1;

  // clear IRQ flag
  *TIMER_IRQ_CLR = 0;

  // start timer
  *TIMER_CONTROL |= 0x000000A2;

  // enable interrupts
  *INTERRUPT_ENABLE_BASIC_IRQS = 0x01;

  // enable IRQ
  _enable_IRQ();
}

void SetTimer(int timerId, int timeout, unsigned char data) {
  _disable_IRQ();
  timerctl.timer[timerId].timeout = timeout;
  timerctl.timer[timerId].data = data;
  _enable_IRQ();
}
