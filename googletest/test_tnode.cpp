#include "../src/btree.h"
#include "../src/key.h"
#include "../src/node.h"
#include <gtest/gtest.h>



TEST(TNodeTest, Constructor) {
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

TEST(TNodeTest, Insert) {
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

TEST(TNodeTest, InsertAndSearch) {
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


TEST(TNodeTest, WriteBtreeKeyIsLeafFalse) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodewritebtreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.header.keycount, 0);
  Key<int> write_key(2);
  m_tnode.WriteBtreeKey(&write_key);
  // TODO: What can we test here?
  // Where is the data being written and what should it have?
  remove((m_filename+".ndx").c_str());
}

TEST(TNodeTest, WriteBtreeKeyIsLeafTrue) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodewritebtreetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  TNode m_tnode(&btree, m_nbr);
  EXPECT_EQ(m_tnode.header.keycount, 0);
  m_tnode.header.isleaf = true;
  Key<int> write_key(2);
  m_tnode.WriteBtreeKey(&write_key);
  // TODO: What can we test here?
  // Where is the data being written and what should it have?
  remove((m_filename+".ndx").c_str());
}


TEST(TNodeTest, Deconstructor) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  auto m_tnode = new TNode(&btree, m_nbr);
  delete m_tnode;
  remove((m_filename+".ndx").c_str());
}


TEST(TNodeTest, DeconstructorNodechanged) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  auto m_tnode = new TNode(&btree, m_nbr);
  m_tnode->nodechanged = true;
  delete m_tnode;
  remove((m_filename+".ndx").c_str());
}


TEST(TNodeTest, DeconstructorManyKeys) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  auto m_tnode = new TNode(&btree, m_nbr);
  Key<int> insert_key1(2);
  Key<int> insert_key2(4);
  m_tnode->Insert(&insert_key1);
  m_tnode->Insert(&insert_key2);
  m_tnode->nodechanged = false;
  delete m_tnode;
  remove((m_filename+".ndx").c_str());
}

TEST(TNodeTest, DeconstructorManyKeysNodeChanged) {
  NodeNbr m_nbr(1);
  std::string m_filename = "tnodetest";
  IndexFile m_idxfile(m_filename);
  Class m_class{};
  int i = 1;
  Key<int> m_key (i);
  PdyBtree btree(m_idxfile, &m_class, &m_key);
  auto m_tnode = new TNode(&btree, m_nbr);
  Key<int> insert_key1(2);
  Key<int> insert_key2(4);
  m_tnode->Insert(&insert_key1);
  m_tnode->Insert(&insert_key2);
  m_tnode->nodechanged = true;
  delete m_tnode;
  remove((m_filename+".ndx").c_str());
}
