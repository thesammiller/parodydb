#include <gtest/gtest.h>
#include "../src/btree.h"

TEST(TestBTree, TreeheaderConstructor) {
  TreeHeader treeheader;
  EXPECT_EQ(treeheader.rootnode, 0);
  EXPECT_EQ(treeheader.keylength, 0);
}

TEST(TestBTree, BTree) {
  TreeHeader treeheader;
  EXPECT_EQ(treeheader.rootnode, 0);
  EXPECT_EQ(treeheader.keylength, 0);
}

