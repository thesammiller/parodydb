#include "../src/linklist.h"

#include <gtest/gtest.h>
#include <mach/vm_types.h>

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

TEST(LinkedList, ClearEntryNoEntry) {
  LinkedList<int> list;
  list.ClearList();
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.iterator), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.firstentry), 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(list.lastentry), 0);
}

TEST(LinkedList, NoCurrentEntry) {
  LinkedList<int> list;
  auto currententry = list.CurrentEntry();
  EXPECT_EQ(0, currententry);
}

TEST(LinkedList, NextEntryCurrentEntry) {
  LinkedList<int> list;
  int i = 1;
  list.AppendEntry(&i);
  list.NextEntry();
  auto currententry = list.CurrentEntry();
  EXPECT_EQ(list.iterator->thisentry, currententry);
}

TEST(LinkedList, DoubleNext) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.NextEntry();
  list.NextEntry();
  auto currententry = list.CurrentEntry();
  EXPECT_EQ(j, *currententry);
}


TEST(LinkedList, FirstEntry) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  auto currententry = list.FirstEntry();;
  EXPECT_EQ(i, *currententry);
}

TEST(LinkedList, LastEntry) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  auto currententry = list.LastEntry();;
  EXPECT_EQ(j, *currententry);
}

TEST(LinkedList, FindEntry) {
  LinkedList<int> list;
  int i = 1;
  list.AppendEntry(&i);
  int pos = list.FindEntry(&i);
  EXPECT_EQ(pos, 0);
}

TEST(LinkedList, FindEntryTwo) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  int pos = list.FindEntry(&j);
  EXPECT_EQ(pos, 1);
}

TEST(LinkedList, NextEntrySkip) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.NextEntry(&i);
  auto currententry = list.LastEntry();;
  EXPECT_EQ(j, *currententry);
}


TEST(LinkedList, PrevEntry) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.NextEntry();
  list.NextEntry();
  auto currententry = list.PrevEntry();
  EXPECT_EQ(i, *currententry);
}

TEST(LinkedList, PrevEntryNoNext) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  auto currententry = list.PrevEntry();
  EXPECT_EQ(j, *currententry);
}

TEST(LinkedList, PrevEntrySkip) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  auto currententry = list.PrevEntry(&j);
  EXPECT_EQ(i, *currententry);
}

TEST(LinkedList, RemoveEntry) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  EXPECT_EQ(list.firstentry->thisentry, &i);
  list.AppendEntry(&j);
  list.RemoveEntry(&i);
  EXPECT_EQ(list.firstentry->thisentry, &j);
}

TEST(LinkedList, RemoveEntryIteratorNotZero) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  list.AppendEntry(&i);
  EXPECT_EQ(list.firstentry->thisentry, &i);
  list.AppendEntry(&j);
  list.NextEntry();
  list.NextEntry();
  list.RemoveEntry(&i);
  EXPECT_EQ(list.firstentry->thisentry, &j);
}

TEST(LinkedList, RemoveEntryLentryZero) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  ListEntry<int> *nullentry = 0;
  list.RemoveEntry(nullentry);
}

TEST(LinkedList, RemoveEntryLentryIterator) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.AppendEntry(&k);
  list.NextEntry();
  list.NextEntry();
  list.RemoveEntry(list.iterator);
  EXPECT_EQ(list.firstentry->thisentry, &i);
  EXPECT_EQ(list.lastentry->thisentry, &k);
}

TEST(LinkedList, RemoveEntryLentryNext) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.AppendEntry(&k);
  list.NextEntry();
  list.NextEntry();
  list.RemoveEntry(list.firstentry);
  EXPECT_EQ(list.firstentry->thisentry, &j);
  EXPECT_EQ(list.firstentry->nextentry, list.lastentry);
  EXPECT_EQ(list.lastentry->preventry, list.firstentry);
  EXPECT_EQ(list.lastentry->thisentry, &k);
}


TEST(LinkedList, RemoveEntryLentryPrev) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.AppendEntry(&k);
  list.NextEntry();
  list.NextEntry();
  list.RemoveEntry(list.lastentry);
  EXPECT_EQ(list.firstentry->thisentry, &i);
  EXPECT_EQ(list.firstentry->nextentry, list.lastentry);
  EXPECT_EQ(list.lastentry->preventry, list.firstentry);
  EXPECT_EQ(list.lastentry->thisentry, &j);
}


TEST(LinkedList, RemoveEntryLentryValue) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.AppendEntry(&k);
  list.RemoveEntry(&j);
  EXPECT_EQ(list.firstentry->thisentry, &i);
  EXPECT_EQ(list.firstentry->nextentry, list.lastentry);
  EXPECT_EQ(list.lastentry->preventry, list.firstentry);
  EXPECT_EQ(list.lastentry->thisentry, &k);
}



TEST(LinkedList, InsertEntry) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.InsertEntry(&k);
  list.NextEntry();
  list.NextEntry();
  list.NextEntry();
  EXPECT_EQ(list.iterator->thisentry, &k);
}

TEST(LinkedList, InsertEntryLentryPos) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.InsertEntry(&k, (short int) 0);
  EXPECT_EQ(list.firstentry->thisentry, &k);
}

TEST(LinkedList, InsertEntryLentryPosOne) {
  LinkedList<int> list;
  int i = 1;
  int j = 2;
  int k = 3;
  list.AppendEntry(&i);
  list.AppendEntry(&j);
  list.NextEntry();
  list.NextEntry();
  list.InsertEntry(&k, (short int) 1);
  EXPECT_EQ(list.iterator->thisentry, &k);
}


// FindEntry
// This seems to be covered by some of the others, like
// Remove and Insert
// TODO: Complete this as interested/desired/needed


