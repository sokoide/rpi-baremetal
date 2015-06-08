#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// called by _IRQ_interrupt in startup.s
void IRQ_handler(void) {
  _disable_IRQ();

  int i;

  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    timerctl.counter++;

    // TODO: should use list
    for (i = 0; i < timerctl.length; i++) {
      if (timerctl.timer[i].timeout <= timerctl.counter) {
        PutFifo8(timerctl.fifo, timerctl.timer[i].data);
        timerctl.timer[i].timeout = 0xffffffff;
      }
    }

    // recalcurate next
    timerctl.next = 0xffffffff;
    for (i = 0; i < timerctl.length; i++) {
      if (timerctl.next > timerctl.timer[i].timeout) {
        timerctl.next = timerctl.timer[i].timeout;
      }
    }

    // clear interrupt flag
    *TIMER_IRQ_CLR = 0;
  }

  _enable_IRQ();
}

void draw_counter(int y, int counter) {
  char line[256];

  // clear the 1st line
  FillRect(0, y, kWidth, 16, 0);

  sprintf(line, "counter: %d", counter);
  PrintStr(0, y, "*", 1);
  PrintStr(16, y, line, 7);
}

int main(int argc, char const* argv[]) {
  rpiInit();
  FramebufferInitialize();
  _init_vector_table();

  FIFO8 fifoTimer;
  unsigned char bufTimer[64];
  const unsigned char timerData1 = 1;
  const unsigned char timerData2 = 2;
  const unsigned char timerData3 = 3;
  uint32_t counter1 = 0;
  uint32_t counter2 = 0;
  uint32_t counter3 = 0;

  int timerInterval1 = 100;
  int timerInterval2 = 500;
  int timerInterval3 = 1000;
  TIMER *timer1, *timer2, *timer3;

  InitFifo8(&fifoTimer, sizeof(bufTimer) / sizeof(unsigned char), bufTimer);
  InitTimer(&fifoTimer);
  timer1 = SetTimer(NULL, timerInterval1, timerData1);
  timer2 = SetTimer(NULL, timerInterval2, timerData2);
  timer3 = SetTimer(NULL, timerInterval3, timerData3);

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
          break;
        case (const int)timerData2:
          counter2++;
          SetTimer(timer2, timerInterval2, timerData2);
          draw_counter(16, counter2);
          break;
        case (const int)timerData3:
          counter3++;
          SetTimer(timer3, timerInterval3, timerData3);
          draw_counter(32, counter3);
          break;
      }
    }
  }
  return 0;
}
