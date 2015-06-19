#include "rpi.h"
#include <stdio.h>
#include <stdlib.h>

THREADCTL threadctl;

void InitThread() {
  threadctl.length = 1;
  threadctl.thread[0].state = THREAD_RUNNING;
  threadctl.currentId = 0;

  threadctl.thread[0].stack = NULL;
  /* threadctl.thrad[0].stackSize = TBD; */
}

void CreateThread(void *thread_entry) {
  unsigned int id = threadctl.length;
  const unsigned int stackSize = 4096;  // 4096 * sizeof(int) allocated
  int *stackBase;

  threadctl.length++;
  threadctl.thread[id].state = THREAD_CREATED;
  stackBase = (int *)malloc(stackSize * sizeof(int));
  stackBase += stackSize - 1;
  threadctl.thread[id].stack = stackBase;
  threadctl.thread[id].stackSize = stackSize;

  // push default registers into the stack which will be poped in
  // _context_switch
  *threadctl.thread[id].stack-- = 0x00000093;  // cpsr =IRQ enabled/FIQ & Thumb
                                               // disabled/Processor mode = SVC
  *threadctl.thread[id].stack-- =
      (int)thread_entry;              // r14: lr (to be stored in pc)
  *threadctl.thread[id].stack-- = 0;  // r12
  *threadctl.thread[id].stack-- = 0;  // r11
  *threadctl.thread[id].stack-- = 0;  // r10
  *threadctl.thread[id].stack-- = 0;  // r9
  *threadctl.thread[id].stack-- = 0;  // r8
  *threadctl.thread[id].stack-- = 0;  // r7
  *threadctl.thread[id].stack-- = 0;  // r6
  *threadctl.thread[id].stack-- = 0;  // r5
  *threadctl.thread[id].stack-- = 0;  // r4
  *threadctl.thread[id].stack-- = 0;  // r3
  *threadctl.thread[id].stack-- = 0;  // r2
  *threadctl.thread[id].stack-- = 0;  // r1
  *threadctl.thread[id].stack = 0;    // r0
  // don't do stack--!
}

unsigned int ContextSwitch(int sp) {
  if (threadctl.length <= 1) {
    return 0xffffffff;
  }

  unsigned int currentId = threadctl.currentId;
  unsigned int nextId = currentId + 1;

  if (nextId >= threadctl.length) {
    nextId = 0;
  }

  char message[512];
  // get current sp
  threadctl.thread[currentId].stack = ((unsigned int *)sp) - 15;

  sprintf(message, "during ContextSwith(), sp=%x, current=%d:%p, next=%d:%p",
          sp, currentId, threadctl.thread[currentId].stack, nextId,
          threadctl.thread[nextId].stack);
  FillRect(0, 32, kWidth, 16, 0);
  PrintStr(0, 32, message, 7);

  // do context switch
  threadctl.thread[nextId].state = THREAD_RUNNING;
  threadctl.thread[currentId].state = THREAD_WAITING;
  threadctl.currentId = nextId;
  return nextId;
}
