#include "../src/linklist.h"

#include <gtest/gtest.h>

TEST(ListEntry, Construct) {
  int i = 1;
  ListEntry<int> entry(&i);
  EXPECT_EQ(&i, entry.thisentry);
}

TEST(LinkedList, Construct) {
  LinkedList<int> list;
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.iterator), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.firstentry), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.lastentry), 0);
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

TEST(LinkedList, ClearEntry) {
  LinkedList<int> list;
  int i = 1;
  list.AppendEntry(&i);
  EXPECT_EQ(&i, list.firstentry->thisentry);
  int j = 2;
  list.ClearList();
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.iterator), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.firstentry), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.lastentry), 0);
}


