#include "gtest/gtest.h"
#include "rpi.h"

TEST(CollectionCase, InitListTimer) {
  ListTimer l;
  InitListTimer(&l);

  EXPECT_EQ(NULL, l.head);
}

TEST(CollectionCase, InsertListTimer1) {
  ListTimer l;
  TIMER timer1 = {.timeout = 500, .data = 5};

  InitListTimer(&l);
  InsertListTimer(&l, &timer1);

  ASSERT_TRUE(NULL != l.head);

  TIMER result = TimerAt(&l, 0);
  EXPECT_EQ(timer1.timeout, result.timeout);
  EXPECT_EQ(timer1.data, result.data);
}

TEST(CollectionCase, InsertListTimer5) {
  ListTimer l;
  TIMER timer1 = {.timeout = 500, .data = 1};
  TIMER timer2 = {.timeout = 100, .data = 2};
  TIMER timer3 = {.timeout = 1000, .data = 3};
  TIMER timer4 = {.timeout = 700, .data = 4};
  TIMER timer5 = {.timeout = 800, .data = 5};

  InitListTimer(&l);
  InsertListTimer(&l, &timer1);
  InsertListTimer(&l, &timer2);
  InsertListTimer(&l, &timer3);
  InsertListTimer(&l, &timer4);
  InsertListTimer(&l, &timer5);

  ASSERT_TRUE(NULL != l.head);
  ASSERT_EQ(5, CountListTimer(&l));

  TIMER result = TimerAt(&l, 0);
  EXPECT_EQ(timer2.timeout, result.timeout);
  EXPECT_EQ(timer2.data, result.data);
  result = TimerAt(&l, 1);
  EXPECT_EQ(timer1.timeout, result.timeout);
  EXPECT_EQ(timer1.data, result.data);
  result = TimerAt(&l, 2);
  EXPECT_EQ(timer4.timeout, result.timeout);
  EXPECT_EQ(timer4.data, result.data);
  result = TimerAt(&l, 3);
  EXPECT_EQ(timer5.timeout, result.timeout);
  EXPECT_EQ(timer5.data, result.data);
  result = TimerAt(&l, 4);
  EXPECT_EQ(timer3.timeout, result.timeout);
  EXPECT_EQ(timer3.data, result.data);
}

TEST(CollectionCase, CountListTimer) {
  ListTimer l;
  TIMER timer;
  TIMER timer2;

  InitListTimer(&l);
  PrependListTimer(&l, &timer);
  PrependListTimer(&l, &timer2);

  int result = CountListTimer(&l);

  EXPECT_EQ(2, result);
}
