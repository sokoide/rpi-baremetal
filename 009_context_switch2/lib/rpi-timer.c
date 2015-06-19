#include "rpi.h"

TIMERCTL timerctl;

void InitTimerInterrupt(FIFO8* fifo) {
  InitTimerCtl(fifo);

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

void InitTimerCtl(FIFO8* fifo) {
  timerctl.counter = 0;
  timerctl.next = 0xffffffff;
  timerctl.length = 0;
  timerctl.fifo = fifo;
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
}

int SetTimer(int id, unsigned int timeout, unsigned char data) {
  _disable_IRQ();
  unsigned int updatedTimeout = timerctl.counter + timeout;

  TIMER* t = &timerctl.timer[id];
  t->timeout = updatedTimeout;
  t->data = data;
  t->used = true;

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
