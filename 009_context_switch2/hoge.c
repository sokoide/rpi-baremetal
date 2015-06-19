#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// called by _IRQ_interrupt in startup.s
int* IRQ_handler(int lr, int sp) {
  bool shouldDoContextSwitch = false;

  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    timerctl.counter++;

    TIMER *p, *prev;
    for (p = timerctl.head; NULL != p; p = p->next) {
      if (p->timeout <= timerctl.counter) {
        if (p->data == 0) {
          shouldDoContextSwitch = true;
          p->timeout = timerctl.counter + 1000;
          timerctl.length--;
        } else {
          PutFifo8(timerctl.fifo, p->data);
          p->timeout = 0xffffffff;
          p->used = false;
          timerctl.length--;
        }
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

    if (true == shouldDoContextSwitch) {
      InsertTimer(0);
    }
  }

  char message[512];

  if (false == shouldDoContextSwitch) {
    // clear interrupt flag
    *TIMER_IRQ_CLR = 0;
    return NULL;
  }
  unsigned int currentId = threadctl.currentId;
  int y = 128;

  sprintf(message, "IRQ_handler, lr=0x%x, sp=0x%x", lr, sp);
  FillRect(0, y, kWidth, 16, 0);
  PrintStr(0, y, message, 7);
  y += 16;

  sprintf(message, "before ContextSwith(), sp=0x%x, current=%d:%p", sp,
          currentId, threadctl.thread[currentId].stack);
  FillRect(0, y, kWidth, 16, 0);
  PrintStr(0, y, message, 7);
  y += 16;

  unsigned int nextId = ContextSwitch(sp);

  sprintf(message, "after ContextSwith(), sp=%x, current=%d:%p, next=%d:%p", sp,
          currentId, threadctl.thread[currentId].stack, nextId,
          threadctl.thread[nextId].stack);
  FillRect(0, y, kWidth, 16, 0);
  PrintStr(0, y, message, 7);
  y += 16;
  sprintf(message, "r0=%x,r1=%x,r2=%x,r3=%x,r4=%x,r5=%x,r6=%x",
          *(threadctl.thread[nextId].stack),
          *(threadctl.thread[nextId].stack + 1),
          *(threadctl.thread[nextId].stack + 2),
          *(threadctl.thread[nextId].stack + 3),
          *(threadctl.thread[nextId].stack + 4),
          *(threadctl.thread[nextId].stack + 5),
          *(threadctl.thread[nextId].stack + 6));
  FillRect(0, y, kWidth, 16, 0);
  PrintStr(0, y, message, 7);
  y += 16;
  sprintf(message,
          "r7=%x,r8=%x,r9=%x,r10=%x,r11=%x,r12=%x,lr=0x%x,pc=0x%x,cpsr=0x%x",
          *(threadctl.thread[nextId].stack + 7),
          *(threadctl.thread[nextId].stack + 8),
          *(threadctl.thread[nextId].stack + 9),
          *(threadctl.thread[nextId].stack + 10),
          *(threadctl.thread[nextId].stack + 11),
          *(threadctl.thread[nextId].stack + 12),
          *(threadctl.thread[nextId].stack + 13),
          *(threadctl.thread[nextId].stack + 14),
          *(threadctl.thread[nextId].stack + 15));
  FillRect(0, y, kWidth, 16, 0);
  PrintStr(0, y, message, 7);

  if (0xffffffff == nextId) {
    // clear interrupt flag
    *TIMER_IRQ_CLR = 0;
    return NULL;
  }
  // clear interrupt flag
  *TIMER_IRQ_CLR = 0;
  return threadctl.thread[nextId].stack;
}

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
  unsigned int counter = 0;
  while (true) {
    draw_counter(1, counter++);
  }
}

void task_b() {
  unsigned int counter = 0;
  while (true) {
    draw_counter(2, counter++);
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
  int timerInterval1 = 1000;
  int timerInterval2 = 10;
  unsigned int counter2 = 0;

  InitFifo8(&fifoTimer, sizeof(bufTimerFifo) / sizeof(unsigned char),
            bufTimerFifo);
  InitTimerInterrupt(&fifoTimer);
  timer1 = CreateTimer();
  timer2 = CreateTimer();

  // create threads
  _disable_IRQ();
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
  _enable_IRQ();

  SetTimer(timer1, timerInterval1, timerData1);
  SetTimer(timer2, timerInterval2, timerData2);

  while (true) {
    _disable_IRQ();
    _wfi();
    _enable_IRQ();

    if (StatusFifo8(&fifoTimer) == 0) {
      _enable_IRQ();
    } else {
      unsigned char data = GetFifo8(&fifoTimer);
      _enable_IRQ();

      switch (data) {
        case (const int)timerData2:
          counter2++;
          SetTimer(timer2, timerInterval2, timerData2);
          draw_counter(0, counter2);
          break;
      }
    }
  }

  return 0;
}
