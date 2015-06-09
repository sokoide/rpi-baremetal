#include "gtest/gtest.h"
#include "rpi.h"

TEST(Fifo8Case, Init) {
  FIFO8 fifo;
  unsigned char buf[16];

  InitFifo8(&fifo, 16, buf);
  EXPECT_EQ(16, fifo.size);
  EXPECT_EQ(buf, fifo.buf);
  EXPECT_EQ(16, fifo.free);
  EXPECT_EQ(0, fifo.p);
  EXPECT_EQ(0, fifo.q);
  EXPECT_EQ(0, fifo.flags);
}

TEST(Fifo8Case, Put) {
  FIFO8 fifo;
  unsigned char buf[16];

  InitFifo8(&fifo, 16, buf);
  PutFifo8(&fifo, 42);

  int result = StatusFifo8(&fifo);
  EXPECT_EQ(1, result);
}

TEST(Fifo8Case, PutGet) {
  FIFO8 fifo;
  unsigned char buf[16];

  InitFifo8(&fifo, 16, buf);
  PutFifo8(&fifo, 42);

  int result = GetFifo8(&fifo);
  EXPECT_EQ(42, result);
}

TEST(Fifo8Case, Put4Get2) {
  FIFO8 fifo;
  unsigned char buf[16];

  InitFifo8(&fifo, 16, buf);
  PutFifo8(&fifo, 42);
  PutFifo8(&fifo, 43);
  PutFifo8(&fifo, 44);
  PutFifo8(&fifo, 45);

  int result = GetFifo8(&fifo);
  EXPECT_EQ(42, result);

  result = GetFifo8(&fifo);
  EXPECT_EQ(43, result);

  result = StatusFifo8(&fifo);
  EXPECT_EQ(2, result);
}

TEST(Fifo8Case, PutOverflow) {
  FIFO8 fifo;
  unsigned char buf[2];
  int result;

  InitFifo8(&fifo, 2, buf);
  result = PutFifo8(&fifo, 42);
  EXPECT_EQ(0, result);
  result = PutFifo8(&fifo, 43);
  EXPECT_EQ(0, result);
  result = PutFifo8(&fifo, 44);
  EXPECT_EQ(-1, result);
}
