#include "rpi.h"
#include <stdio.h>

TIMERCTL timerctl;

// called by _IRQ_interrupt in startup.s
int* IRQ_handler(int lr, int sp) {
  if (*INTERRUPT_IRQ_BASIC_PENDING & 0x01 != 0) {
    timerctl.counter++;

    TIMER* p;
    for (p = timerctl.head; NULL != p; p = p->next) {
      if (p->timeout <= timerctl.counter) {
        PutFifo8(p->fifo, p->data);
        p->timeout = 0xffffffff;
        p->used = false;
        timerctl.length--;
      } else {
        break;
      }
    }

    // remove signaled timers from the list
    timerctl.head = p;

    // recalculate next
    if (timerctl.length > 0) {
      timerctl.next = timerctl.head->timeout;
    } else {
      timerctl.next = 0xffffffff;
    }

    char message[512];

    if (0 != timerctl.counter % 16) {
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

    sprintf(message, "after ContextSwith(), sp=%x, current=%d:%p, next=%d:%p",
            sp, currentId, threadctl.thread[currentId].stack, nextId,
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
    sprintf(message, "r7=%x,r8=%x,r9=%x,r10=%x,r11=%x,r12=%x,lr=0x%x,pc=0x%x",
            *(threadctl.thread[nextId].stack + 7),
            *(threadctl.thread[nextId].stack + 8),
            *(threadctl.thread[nextId].stack + 9),
            *(threadctl.thread[nextId].stack + 10),
            *(threadctl.thread[nextId].stack + 11),
            *(threadctl.thread[nextId].stack + 12),
            *(threadctl.thread[nextId].stack + 13),
            *(threadctl.thread[nextId].stack + 14));
    FillRect(0, y, kWidth, 16, 0);
    PrintStr(0, y, message, 7);
    y += 16;
    sprintf(message, "cpsr=0x%x", *(threadctl.thread[nextId].stack + 15));
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
}

void InitTimerInterrupt() {
  InitTimerCtl();

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

void InitTimerCtl() {
  timerctl.counter = 0;
  timerctl.next = 0xffffffff;
  timerctl.length = 0;
  timerctl.head = NULL;

  for (int i = 0; i < MAX_TIMERS; i++) {
    InitTimer(i);
  }
}

int CreateTimer() {
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (false == timerctl.timer[i].used) {
      timerctl.timer[i].used = true;
      return i;
    }
  }
  return -1;
}

void DeleteTimer(int id) {
  RemoveTimer(id);
  InitTimer(id);
}

void InitTimer(int id) {
  // init
  timerctl.timer[id].id = id;
  timerctl.timer[id].timeout = 0xffffffff;
  timerctl.timer[id].data = 0;
  timerctl.timer[id].used = false;
  timerctl.timer[id].next = NULL;
  timerctl.timer[id].fifo = NULL;
}

int SetTimer(FIFO8* fifo, int id, unsigned int timeout, unsigned char data) {
  _disable_IRQ();
  unsigned int updatedTimeout = timerctl.counter + timeout;

  TIMER* t = &timerctl.timer[id];
  t->timeout = updatedTimeout;
  t->data = data;
  t->used = true;
  t->fifo = fifo;

  InsertTimer(id);

  if (timerctl.next > updatedTimeout) {
    timerctl.next = updatedTimeout;
  }

  _enable_IRQ();
  return id;
}

// insert based on timer->timeout
void InsertTimer(int id) {
  TIMER* timer = &timerctl.timer[id];
  timer->used = true;
  timerctl.length++;

  // if empty
  if (NULL == timerctl.head) {
    timerctl.head = timer;
    timer->next = NULL;
    return;
  }

  TIMER *p, *prev;
  for (p = timerctl.head, prev = NULL; NULL != p; p = p->next) {
    if (p->timeout >= timer->timeout) {
      if (NULL == prev) {
        timerctl.head = timer;
      } else {
        prev->next = timer;
      }
      timer->next = p;
      return;
    }
    prev = p;
  }
  prev->next = timer;
  timer->next = NULL;
  return;
}

bool RemoveTimer(int id) {
  TIMER *p, *prev;

  for (p = timerctl.head, prev = NULL; NULL != p; p = p->next) {
    if (p->id == id) {
      if (NULL == prev) {
        timerctl.head = p->next;
      } else {
        prev->next = p->next;
      }
      p->used = false;
      timerctl.length--;
      return true;
    }
    prev = p;
  }
  return false;
}

TIMER* TimerAt(int index) {
  TIMER* t = timerctl.head;
  for (int i = 0; i < index; i++) {
    t = t->next;
  }
  return t;
}
