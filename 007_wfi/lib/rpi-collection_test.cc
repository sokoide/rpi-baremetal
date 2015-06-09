#include "gtest/gtest.h"
#include "rpi.h"

TEST(CollectionCase, InitListTimer) {
  ListTimer l;
  InitListTimer(&l);

  EXPECT_EQ(NULL, l.head);
  CleanupListTimer(&l);
}

TEST(CollectionCase, InsertListTimer1) {
  ListTimer l;
  TIMER timer0 = {.timeout = 500, .data = 5};

  InitListTimer(&l);
  InsertListTimer(&l, &timer0);

  ASSERT_TRUE(NULL != l.head);

  TIMER result = TimerAt(&l, 0);
  EXPECT_EQ(timer0.timeout, result.timeout);
  EXPECT_EQ(timer0.data, result.data);
  CleanupListTimer(&l);
}

TEST(CollectionCase, InsertListTimer5) {
  ListTimer l;
  TIMER timer0 = {.timeout = 500, .data = 1};
  TIMER timer1 = {.timeout = 100, .data = 2};
  TIMER timer2 = {.timeout = 1000, .data = 3};
  TIMER timer3 = {.timeout = 700, .data = 4};
  TIMER timer4 = {.timeout = 800, .data = 5};

  int id;
  InitListTimer(&l);
  id = InsertListTimer(&l, &timer0);
  EXPECT_EQ(0, id);
  id = InsertListTimer(&l, &timer1);
  EXPECT_EQ(1, id);
  id = InsertListTimer(&l, &timer2);
  EXPECT_EQ(2, id);
  id = InsertListTimer(&l, &timer3);
  EXPECT_EQ(3, id);
  id = InsertListTimer(&l, &timer4);
  EXPECT_EQ(4, id);

  ASSERT_TRUE(NULL != l.head);
  ASSERT_EQ(5, CountListTimer(&l));

  TIMER result = TimerAt(&l, 0);
  EXPECT_EQ(timer1.timeout, result.timeout);
  EXPECT_EQ(timer1.data, result.data);
  result = TimerAt(&l, 1);
  EXPECT_EQ(timer0.timeout, result.timeout);
  EXPECT_EQ(timer0.data, result.data);
  result = TimerAt(&l, 2);
  EXPECT_EQ(timer3.timeout, result.timeout);
  EXPECT_EQ(timer3.data, result.data);
  result = TimerAt(&l, 3);
  EXPECT_EQ(timer4.timeout, result.timeout);
  EXPECT_EQ(timer4.data, result.data);
  result = TimerAt(&l, 4);
  EXPECT_EQ(timer2.timeout, result.timeout);
  EXPECT_EQ(timer2.data, result.data);
  CleanupListTimer(&l);
}

TEST(CollectionCase, RemoveListTimer) {
  ListTimer l;
  TIMER timer0 = {.timeout = 500, .data = 1};
  TIMER timer1 = {.timeout = 100, .data = 2};
  TIMER timer2 = {.timeout = 1000, .data = 3};
  TIMER timer3 = {.timeout = 700, .data = 4};
  TIMER timer4 = {.timeout = 800, .data = 5};

  InitListTimer(&l);
  InsertListTimer(&l, &timer0);
  InsertListTimer(&l, &timer1);
  InsertListTimer(&l, &timer2);
  InsertListTimer(&l, &timer3);
  InsertListTimer(&l, &timer4);

  ASSERT_TRUE(NULL != l.head);
  ASSERT_EQ(5, CountListTimer(&l));

  // remove id=3, timer3
  bool r = RemoveListTimer(&l, 3);
  ASSERT_TRUE(r);
  ASSERT_EQ(4, CountListTimer(&l));

  TIMER result = TimerAt(&l, 0);
  EXPECT_EQ(timer1.timeout, result.timeout);
  EXPECT_EQ(timer1.data, result.data);
  result = TimerAt(&l, 1);
  EXPECT_EQ(timer0.timeout, result.timeout);
  EXPECT_EQ(timer0.data, result.data);
  result = TimerAt(&l, 2);
  EXPECT_EQ(timer4.timeout, result.timeout);
  EXPECT_EQ(timer4.data, result.data);
  result = TimerAt(&l, 3);
  EXPECT_EQ(timer2.timeout, result.timeout);
  EXPECT_EQ(timer2.data, result.data);
  CleanupListTimer(&l);
}

TEST(CollectionCase, CountListTimer) {
  ListTimer l;
  TIMER timer0;
  TIMER timer1;

  InitListTimer(&l);
  InsertListTimer(&l, &timer0);
  InsertListTimer(&l, &timer1);

  int result = CountListTimer(&l);

  EXPECT_EQ(2, result);
  CleanupListTimer(&l);
}
