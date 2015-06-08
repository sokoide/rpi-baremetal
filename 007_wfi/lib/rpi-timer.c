#include "rpi.h"

TIMERCTL timerctl;

void InitTimer(FIFO8* fifo) {
  // init timerctl;
  timerctl.counter = 0;
  timerctl.next = 0xffffffff;
  timerctl.length = 0;
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

TIMER* SetTimer(TIMER* timer, unsigned int timeout, unsigned char data) {
  _disable_IRQ();

  // TODO: should use list
  if (NULL == timer) {
    timer = &timerctl.timer[timerctl.length++];
  }

  timer->timeout = timerctl.counter + timeout;
  timer->data = data;

  if (timerctl.next > timer->timeout) {
    timerctl.next = timer->timeout;
  }

  _enable_IRQ();
  return timer;
}
