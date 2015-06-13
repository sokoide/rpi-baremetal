#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// called by _IRQ_interrupt in startup.s
void IRQ_handler(void) {
  _disable_IRQ();

  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    timerctl.counter++;

    TIMER *p, *prev;
    for (p = timerctl.head; NULL != p; p = p->next) {
      if (p->timeout <= timerctl.counter) {
        PutFifo8(timerctl.fifo, p->data);
        p->timeout = 0xffffffff;
        p->used = false;
        timerctl.length--;
      } else {
        break;
      }
    }

    // removed signaled timers from the list
    timerctl.head = p;

    // recalculate next
    if (timerctl.length > 0) {
      timerctl.next = timerctl.head->timeout;
    } else {
      timerctl.next = 0xffffffff;
    }

    // clear interrupt flag
    *TIMER_IRQ_CLR = 0;
  }

  _enable_IRQ();
}

void draw_counter(int threadid, int counter) {
  char line[256];
  int y = 64 + 16 * threadid;

  // clear the line
  FillRect(0, y, kWidth, 16, 0);

  sprintf(line, "[%d] counter: %d", threadid, counter);
  PrintStr(0, y, "*", 1);
  PrintStr(16, y, line, 7);
}

void task_a() {
  unsigned int counter = 0;
  while (true) {
    draw_counter(1, counter++);
    ContextSwitch();
  }
}

void task_b() {
  unsigned int counter = 0;
  while (true) {
    draw_counter(2, counter++);
    ContextSwitch();
  }
}

int main(int argc, char const* argv[]) {
  unsigned int counter = 0;
  rpiInit();
  FramebufferInitialize();
  _init_vector_table();

  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, "* main started", 7);

  FIFO8 fifoTimer;
  unsigned char bufTimerFifo[64];
  const unsigned char timerData1 = 1;
  int timerInterval1 = 1000;
  unsigned int counter1 = 0;
  int timer1;

  InitFifo8(&fifoTimer, sizeof(bufTimerFifo) / sizeof(unsigned char),
            bufTimerFifo);
  InitTimerInterrupt(&fifoTimer);
  timer1 = CreateTimer();
  SetTimer(timer1, timerInterval1, timerData1);

  // create threads
  // main -> thread 0
  InitThread();
  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, "* thread initialized", 7);
  // task_a -> thread 1
  CreateThread(task_a);
  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, "* thread for task_a created", 7);
  // task_b -> thread 2
  CreateThread(task_b);
  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, "* thread for task_b created", 7);

  while (true) {
    _disable_IRQ();
    _wfi();
    if (StatusFifo8(&fifoTimer) == 0) {
      _enable_IRQ();
    } else {
      unsigned char data = GetFifo8(&fifoTimer);
      _enable_IRQ();

      switch (data) {
        case (const int)timerData1:
          counter1++;
          SetTimer(timer1, timerInterval1, timerData1);
          draw_counter(0, counter1);
          ContextSwitch();
          break;
      }
    }
  }

  return 0;
}
