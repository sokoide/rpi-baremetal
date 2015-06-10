#include "gtest/gtest.h"
#include "rpi.h"

TEST(CollectionCase, InitTimerCtl) {
  InitTimerCtl(NULL);
  EXPECT_EQ(0, timerctl.counter);
  EXPECT_EQ(0xffffffff, timerctl.next);
  EXPECT_EQ(0, timerctl.length);
  EXPECT_EQ(NULL, timerctl.fifo);
}

TEST(CollectionCase, InsertTimer1) {
  InitTimerCtl(NULL);
  int id = CreateTimer();
  ASSERT_TRUE(id >= 0);

  timerctl.timer[id].timeout = 500;
  timerctl.timer[id].data = 5;

  InsertTimer(id);
  ASSERT_TRUE(NULL != timerctl.head);

  TIMER *result = TimerAt(0);
  EXPECT_EQ(500, result->timeout);
  EXPECT_EQ(5, result->data);
}

TEST(CollectionCase, InsertTimer5) {
  InitTimerCtl(NULL);
  int id1 = CreateTimer();
  int id2 = CreateTimer();
  int id3 = CreateTimer();
  int id4 = CreateTimer();
  int id5 = CreateTimer();
  ASSERT_TRUE(id1 >= 0);
  ASSERT_TRUE(id2 >= 0);
  ASSERT_TRUE(id3 >= 0);
  ASSERT_TRUE(id4 >= 0);
  ASSERT_TRUE(id5 >= 0);

  timerctl.timer[id1].timeout = 500;
  timerctl.timer[id1].data = 1;
  timerctl.timer[id2].timeout = 100;
  timerctl.timer[id2].data = 2;
  timerctl.timer[id3].timeout = 1000;
  timerctl.timer[id3].data = 3;
  timerctl.timer[id4].timeout = 700;
  timerctl.timer[id4].data = 4;
  timerctl.timer[id5].timeout = 800;
  timerctl.timer[id5].data = 5;

  InsertTimer(id1);
  InsertTimer(id2);
  InsertTimer(id3);
  InsertTimer(id4);
  InsertTimer(id5);

  ASSERT_TRUE(NULL != timerctl.head);
  ASSERT_EQ(5, timerctl.length);

  TIMER *result = TimerAt(0);
  EXPECT_EQ(100, result->timeout);
  EXPECT_EQ(2, result->data);

  result = TimerAt(1);
  EXPECT_EQ(500, result->timeout);
  EXPECT_EQ(1, result->data);

  result = TimerAt(2);
  EXPECT_EQ(700, result->timeout);
  EXPECT_EQ(4, result->data);

  result = TimerAt(3);
  EXPECT_EQ(800, result->timeout);
  EXPECT_EQ(5, result->data);

  result = TimerAt(4);
  EXPECT_EQ(1000, result->timeout);
  EXPECT_EQ(3, result->data);
}

TEST(CollectionCase, RemoveTimer) {
  InitTimerCtl(NULL);
  int id1 = CreateTimer();
  int id2 = CreateTimer();
  int id3 = CreateTimer();
  int id4 = CreateTimer();
  int id5 = CreateTimer();
  ASSERT_TRUE(id1 >= 0);
  ASSERT_TRUE(id2 >= 0);
  ASSERT_TRUE(id3 >= 0);
  ASSERT_TRUE(id4 >= 0);
  ASSERT_TRUE(id5 >= 0);

  timerctl.timer[id1].timeout = 500;
  timerctl.timer[id1].data = 1;
  timerctl.timer[id2].timeout = 100;
  timerctl.timer[id2].data = 2;
  timerctl.timer[id3].timeout = 1000;
  timerctl.timer[id3].data = 3;
  timerctl.timer[id4].timeout = 700;
  timerctl.timer[id4].data = 4;
  timerctl.timer[id5].timeout = 800;
  timerctl.timer[id5].data = 5;

  InsertTimer(id1);
  InsertTimer(id2);
  InsertTimer(id3);
  InsertTimer(id4);
  InsertTimer(id5);

  ASSERT_TRUE(NULL != timerctl.head);
  ASSERT_EQ(5, timerctl.length);

  bool r = RemoveTimer(id4);
  ASSERT_TRUE(r);
  ASSERT_EQ(4, timerctl.length);

  TIMER *result = TimerAt(0);
  EXPECT_EQ(100, result->timeout);
  EXPECT_EQ(2, result->data);

  result = TimerAt(1);
  EXPECT_EQ(500, result->timeout);
  EXPECT_EQ(1, result->data);

  result = TimerAt(2);
  EXPECT_EQ(800, result->timeout);
  EXPECT_EQ(5, result->data);

  result = TimerAt(3);
  EXPECT_EQ(1000, result->timeout);
  EXPECT_EQ(3, result->data);
}
