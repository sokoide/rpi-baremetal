#include "gtest/gtest.h"

TEST(HogeTestCase, HogeTest) {
  int result = 1;
  EXPECT_EQ(1, result);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
