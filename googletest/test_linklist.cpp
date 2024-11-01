#include "../src/linklist.h"

#include <gtest/gtest.h>

TEST(ListEntry, Construct) {
  int i = 1;
  ListEntry<int> entry(&i);
  EXPECT_EQ(&i, entry.thisentry);
}

TEST(LinkedList, Construct) {
  LinkedList<int> list;
  // TODO: Figure out what we test here
}

TEST(LinkedList, AppendEntry) {
  LinkedList<int> list;
  int i = 1;
  list.AppendEntry(&i);
  EXPECT_EQ(&i, list.firstentry->thisentry);
  int j = 2;
  list.AppendEntry(&j);
  EXPECT_EQ(&j, list.lastentry->thisentry);
  EXPECT_EQ(list.firstentry->nextentry, list.lastentry);
  EXPECT_EQ(list.lastentry->preventry, list.firstentry);
}