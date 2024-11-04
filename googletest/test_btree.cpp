#include <gtest/gtest.h>
#include "../src/btree.h"

TEST(TestBTree, TreeheaderConstructor) {
  TreeHeader treeheader;
  EXPECT_EQ(treeheader.rootnode, 0);
  EXPECT_EQ(treeheader.keylength, 0);
}

TEST(TestBTree, BTreeDestructor) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree *btree = new PdyBtree(m_idxfile, &m_class, &m_key);
  delete btree;
  remove((m_filename + ".ndx").c_str());
}

TEST(TestBTree, BTreeInsert) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  btree.Insert(&m_key);
  remove((m_filename + ".ndx").c_str());
}


TEST(TestBTree, BTreeMakeKeyBuffer) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  btree.Insert(&m_key);
  PdyKey *n_key = btree.MakeKeyBuffer();
  EXPECT_EQ(n_key->indexno, 0);
  remove((m_filename + ".ndx").c_str());
}


TEST(TestBTree, SaveKeyBuffer) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  int j = 2;
  Key<int> m_key (i);
  Key<int> n_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  btree.Insert(&m_key);
  btree.Insert(&n_key);
  // Initialize the trnode object
  btree.First();
  btree.trnode->header.isleaf = true;
  btree.SaveKeyPosition();
  EXPECT_EQ(btree.oldcurrnode, 0);
  EXPECT_EQ(btree.oldcurrkey, 0);
  btree.trnode->header.isleaf = false;
  btree.SaveKeyPosition();
  EXPECT_EQ(btree.oldcurrnode, 1);
  EXPECT_EQ(btree.oldcurrkey, 0);
  remove((m_filename + ".ndx").c_str());
}


TEST(TestBTree, FirstFirst) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  int j = 2;
  Key<int> m_key (i);
  Key<int> n_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  btree.Insert(&m_key);
  btree.Insert(&n_key);
  // Initialize the trnode object
  btree.First();
  btree.First();
  Key<int> *x_key =(Key<int>*) btree.Current();
  EXPECT_EQ(x_key->ky, m_key.ky);
  remove((m_filename + ".ndx").c_str());
}



TEST(TestBTree, Last) {
  std::string m_filename = "btreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  int j = 2;
  Key<int> m_key (i);
  Key<int> n_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  btree.Insert(&m_key);
  btree.Insert(&n_key);
  // Initialize the trnode object
  btree.First();
  btree.Last();
  Key<int> *x_key =(Key<int>*) btree.Current();
  EXPECT_EQ(x_key->ky, n_key.ky);
  remove((m_filename + ".ndx").c_str());
}





