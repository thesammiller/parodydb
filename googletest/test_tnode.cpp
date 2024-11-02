#include "../src/btree.h"

#include "../src/btree.h"
#include "../src/key.h"
#include "../src/node.h"
#include <gtest/gtest.h>



TEST(TNodeTest, TestCreateNode) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.header.keycount, 0);
  remove((m_filename+".ndx").c_str());
}

TEST(TNodeTest, TestInsert) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.header.keycount, 0);
  Key<int> insert_key(2);
  m_tnode.Insert(&insert_key);
  EXPECT_EQ(m_tnode.header.keycount, 1);
  remove((m_filename+".ndx").c_str());
}

TEST(TNodeTest, TestInsertAndSearch) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.header.keycount, 0);
  Key<int> insert_key1(2);
  Key<int> insert_key2(4);
  m_tnode.Insert(&insert_key1);
  m_tnode.Insert(&insert_key2);
  EXPECT_EQ(m_tnode.header.keycount, 2);
  auto result = m_tnode.SearchNode(&insert_key2);
  EXPECT_TRUE(result);
  remove((m_filename+".ndx").c_str());
}

TEST(TNodeTest, IsLeafInitiallyFalse) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.isLeaf(), false);
  remove((m_filename+".ndx").c_str());
}


