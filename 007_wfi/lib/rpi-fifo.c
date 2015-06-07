#include "rpi.h"

#define FLAGS_OVERRUN 0x0001

void InitFifo8(FIFO8 *fifo, int size, unsigned char *buf) {
  fifo->size = size;
  fifo->buf = buf;
  fifo->free = size;
  fifo->flags = 0;
  fifo->p = 0;  // for write
  fifo->q = 0;  // for read
  return;
}

int PutFifo8(FIFO8 *fifo, unsigned char data) {
  if (fifo->free == 0) {
    // overflow
    fifo->flags |= FLAGS_OVERRUN;
    return -1;
  }
  fifo->buf[fifo->p] = data;
  fifo->p++;
  if (fifo->p == fifo->size) {
    fifo->p = 0;
  }
  fifo->free--;
  return 0;
}

int GetFifo8(FIFO8 *fifo) {
  int data;
  if (fifo->free == fifo->size) {
    // returns -1 if empty
    return -1;
  }
  data = fifo->buf[fifo->q];
  fifo->q++;
  if (fifo->q == fifo->size) {
    fifo->q = 0;
  }
  fifo->free++;
  return data;
}

int StatusFifo8(FIFO8 *fifo) { return fifo->size - fifo->free; }
