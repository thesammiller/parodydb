//
// Created by Sam Miller on 10/29/24.
//
#include <gtest/gtest.h>
#include "../src/structs.h"

// struct Class
TEST(StructsTest, StructClassConstructor) {
  char *cn = new char[5];
  strcpy(cn, "test");
  Class myClass(cn);
  // Expect two strings not to be equal.
  EXPECT_STREQ(myClass.classname, "test");
  // Expect equality.
  EXPECT_EQ(myClass.classid, 0);
  EXPECT_EQ(myClass.headeraddr, 0);
}

TEST(StructsTest, ObjAddr) {
  int l = 42;
  ObjAddr myObjAddr(l);
  EXPECT_EQ(myObjAddr, l);
}

TEST(StructsTest, ObjectHeader) {
  ObjectHeader myObjHeader;
  EXPECT_EQ(myObjHeader.classid, 0);
  EXPECT_EQ(myObjHeader.ndnbr, 0);
}