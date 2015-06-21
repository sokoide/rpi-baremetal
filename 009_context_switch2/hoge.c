#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void draw_counter(int threadid, int counter) {
  char line[256];
  int y = 64 + 16 * threadid;
  int* sp = (int*)_get_stack_pointer();

  // clear the line
  FillRect(0, y, kWidth, 16, 0);

  sprintf(line, "[Thread:%d] counter: %d", threadid, counter);
  PrintStr(0, y, "*", 1);
  PrintStr(16, y, line, 7);
}

void task_a() {
  unsigned int counter1 = 0;
  FIFO8 fifoTimer;
  unsigned char bufTimerFifo[64];
  const unsigned char timerData1 = 1;
  int timerInterval1 = 100;

  InitFifo8(&fifoTimer, sizeof(bufTimerFifo) / sizeof(unsigned char),
            bufTimerFifo);
  int timer1 = CreateTimer();
  SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);

  while (true) {
    while (StatusFifo8(&fifoTimer) != 0) {
      unsigned char data = GetFifo8(&fifoTimer);
      switch (data) {
        case (const int)timerData1:
          counter1++;
          draw_counter(1, counter1);
          SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);
          break;
      }
    }
  }
}

void task_b() {
  unsigned int counter1 = 0;
  FIFO8 fifoTimer;
  unsigned char bufTimerFifo[64];
  const unsigned char timerData1 = 1;
  int timerInterval1 = 100;

  InitFifo8(&fifoTimer, sizeof(bufTimerFifo) / sizeof(unsigned char),
            bufTimerFifo);
  int timer1 = CreateTimer();
  SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);

  while (true) {
    while (StatusFifo8(&fifoTimer) != 0) {
      unsigned char data = GetFifo8(&fifoTimer);
      switch (data) {
        case (const int)timerData1:
          counter1++;
          draw_counter(2, counter1);
          SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);
          break;
      }
    }
  }
}

int main(int argc, char const* argv[]) {
  int timer1;
  int timer2;
  char message[256];

  rpiInit();
  FramebufferInitialize();
  _init_vector_table();

  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, "* main started", 7);

  // main -> thread 0
  InitThread();
  sprintf(message, "* thread initialized, sp:%p", threadctl.thread[0].stack);
  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, message, 7);

  FIFO8 fifoTimer;
  unsigned char bufTimerFifo[64];
  const unsigned char timerData1 = 0;
  const unsigned char timerData2 = 1;
  int timerInterval1 = 50;  // TODO: Bug when 10 or 20ms, task_b will be skipped
  int timerInterval2 = 100;
  unsigned int counter2 = 0;

  InitFifo8(&fifoTimer, sizeof(bufTimerFifo) / sizeof(unsigned char),
            bufTimerFifo);
  InitTimerInterrupt();
  timer1 = CreateTimer();
  timer2 = CreateTimer();

  // create threads
  // task_a -> thread 1
  CreateThread(task_a);
  sprintf(message, "* thread for task_a created, sp:%x, fp:%p",
          threadctl.thread[1].stack, task_a);
  FillRect(0, 0, kWidth, 16, 0);
  PrintStr(0, 0, message, 7);

  // task_b -> thread 2
  CreateThread(task_b);
  sprintf(message, "* thread for task_b created, sp:%x, fp:%p",
          threadctl.thread[2].stack, task_b);
  FillRect(0, 16, kWidth, 16, 0);
  PrintStr(0, 16, message, 7);

  SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);
  SetTimer(&fifoTimer, timer2, timerInterval2, timerData2);

  while (true) {
    /* _disable_IRQ(); */
    /* _wfi(); */
    /* _enable_IRQ(); */

    while (StatusFifo8(&fifoTimer) > 0) {
      unsigned char data = GetFifo8(&fifoTimer);
      switch (data) {
        case (const int)timerData1:
          SetTimer(&fifoTimer, timer1, timerInterval1, timerData1);
          counter2++;
          break;
        case (const int)timerData2:
          draw_counter(0, counter2);
          SetTimer(&fifoTimer, timer2, timerInterval2, timerData2);
          break;
      }
    }
  }

  return 0;
}
