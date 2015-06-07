#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// called by _IRQ_interrupt in startup.s
void IRQ_handler(void) {
  _disable_IRQ();

  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    timerctl.count++;
    for (int i = 0; i < MAX_TIMER; i++) {
      if (timerctl.timer[i].timeout > 0) {
        if (0 == --timerctl.timer[i].timeout) {
          PutFifo8(timerctl.fifo, timerctl.timer[i].data);
        }
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

int main(int argc, char const *argv[]) {
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

  int timerInterval1 = 1000;
  int timerInterval2 = 500;
  int timerInterval3 = 100;

  InitFifo8(&fifoTimer, sizeof(bufTimer) / sizeof(unsigned char), bufTimer);
  InitTimer(&fifoTimer);
  SetTimer(0, timerInterval1, timerData1);
  SetTimer(1, timerInterval2, timerData2);
  SetTimer(2, timerInterval3, timerData3);

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
          SetTimer(0, timerInterval1, timerData1);
          draw_counter(0, counter1);
          break;
        case (const int)timerData2:
          counter2++;
          SetTimer(1, timerInterval2, timerData2);
          draw_counter(16, counter2);
          break;
        case (const int)timerData3:
          counter3++;
          SetTimer(2, timerInterval3, timerData3);
          draw_counter(32, counter3);
          break;
      }
    }
  }
  return 0;
}
