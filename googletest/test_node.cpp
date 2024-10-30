#include <gtest/gtest.h>
#include "../src/node.h"

TEST(NodeFileTest, TestNodeFileConstructor) {
  std::string filename = "./nodefileconstructor.dat";
  NodeFile testnodefile(filename);
  bool newfile = access(filename.c_str(), 0) != 0;
  remove(filename.c_str());
  EXPECT_EQ(newfile, 0);
}


TEST(NodeFileTest, NodeFileCreateWriteOpen) {
  std::string filename = "./testfile.dat";
  try {
    NodeFile testnodefile(filename);
    testnodefile.header.deletednode = 1;
    testnodefile.header.highestnode = 2;
    testnodefile.WriteData(&(testnodefile.header), sizeof testnodefile.header, 0);
    // This NodeFile has an implicit ReadData
    NodeFile checkNode(filename);
    remove(filename.c_str());
    EXPECT_EQ(checkNode.header.deletednode, testnodefile.header.deletednode);
    EXPECT_EQ(checkNode.header.highestnode, testnodefile.header.highestnode);
  }catch (FileReadError) {
    remove(filename.c_str());
  }
}


TEST(NodeFileTest, DeleteFunctionNoChange) {
  std::string filename = "./deletefunctionnochange.dat";
  auto testnodefile = new NodeFile (filename);
  delete testnodefile;
  NodeFile checkNode(filename);
  remove(filename.c_str());
  EXPECT_EQ(checkNode.header.deletednode, testnodefile->header.deletednode);
  EXPECT_EQ(checkNode.header.highestnode, testnodefile->header.highestnode);
}

TEST(NodeFileTest, DeleteFunction) {
  std::string filename = "./deletefunctionchange.dat";
  try {
    auto testnodefile = new NodeFile(filename);
    testnodefile->header.deletednode = 1;
    testnodefile->header.highestnode = 2;
    int testnodefileHeaderDeleted = testnodefile->header.deletednode;
    int testnodefileHeaderHighest = testnodefile->header.highestnode;
    delete testnodefile;
    NodeFile checkNode(filename);
    remove(filename.c_str());
    EXPECT_EQ(checkNode.header.deletednode, testnodefileHeaderDeleted);
    EXPECT_EQ(checkNode.header.highestnode, testnodefileHeaderHighest);
  }catch (FileReadError) {
    remove(filename.c_str());
    throw FileReadError();
  }
}

TEST(NodeFileTest, NewNodeFunction) {
  std::string filename = "./newnodefunction.dat";
  NodeFile testnodefile(filename);
  NodeNbr newnode = testnodefile.NewNode();
  remove(filename.c_str());
  EXPECT_EQ(testnodefile.header.highestnode, newnode);
}

// TODO: Improve this test based on Node logic
// Dependencies -- Node constructor
//                 NextNode()
//                 SetNextNode()
TEST(NodeFileTest, NewNodeDeletedNode) {
  std::string filename = "./newnodedeletednode.dat";
  NodeFile testnodefile(filename);
  NodeNbr newnode = testnodefile.NewNode();
  remove(filename.c_str());
  EXPECT_EQ(testnodefile.header.highestnode, newnode);
}

TEST(NodeTest, NodeConstructorNodeNbr0) {
  std::string filename = "./nodeconstuctor.dat";
  NodeFile testnodefile(filename);
  Node myNode(&testnodefile, 0);
  remove(filename.c_str());
  EXPECT_EQ(myNode.nodenbr, 0);
}

TEST(NodeTest, NodeConstructorNodeNbr1) {
  std::string filename = "./nodeconstuctor.dat";
  NodeFile testnodefile(filename);
  Node myNode(&testnodefile, 1);
  remove(filename.c_str());
  EXPECT_EQ(myNode.nodenbr, 1);
}

TEST(NodeTest, CloseNode) {
  std::string filename = "./nodeconstuctor.dat";
  NodeFile testnodefile(filename);
  Node node1(&testnodefile, 0);
  node1.CloseNode();
  Node node2(&testnodefile, 1);
  node2.CloseNode();
  Node nodecheck(&testnodefile, 1);
  remove(filename.c_str());
  EXPECT_EQ(1, nodecheck.nodenbr);
}

// TODO: I don't fully understand what is going on
// There seems to be the owner/file holding state
TEST(NodeTest, CloseNodeDeleted) {
  std::string filename = "./nodeconstuctor.dat";
  NodeFile testnodefile(filename);
  Node node1(&testnodefile, 0);
  Node node2(&testnodefile, 1);
  node2.deletenode = true;
  node2.CloseNode();
  Node node3(&testnodefile, 2);
  node1.CloseNode();
  node3.CloseNode();
  Node nodecheck(&testnodefile, 1);
  remove(filename.c_str());
  // TODO: Fix what this is checking with a better understanding
  EXPECT_EQ(1, nodecheck.nodenbr);
}

TEST(NodeTest, OperatorEq) {
  std::string filename1 = "./nodeoperatoreq1.dat";
  NodeFile testnodefile1(filename1);
  std::string filename2 = "./nodeoperatoreq2.dat";
  NodeFile testnodefile2(filename2);
  Node node1(&testnodefile1, 0);
  Node node2(&testnodefile2, 1);
  node1.deletenode = true;
  node1.nodechanged = true;
  node1.deletenode = true;
  node2 = node1;
  // TODO: Fix what this is checking with a better understanding
  EXPECT_EQ(node1.deletenode, node2.deletenode);
  EXPECT_EQ(node1.nodechanged, node2.nodechanged);
  EXPECT_EQ(node1.nodenbr, node2.nodenbr);
  EXPECT_EQ(node1.owner, node2.owner);
  node1.CloseNode();
  node2.CloseNode();
  remove(filename1.c_str());
  remove(filename2.c_str());
}


TEST(NodeTest, NodeAddress) {
  std::string filename = "./nodeoperatoreq.dat";
  NodeFile testnodefile(filename);
  Node node(&testnodefile, 0);
  // TODO: Fix what this is checking with a better understanding
  EXPECT_EQ(node.NodeAddress(), 0);
  node.CloseNode();
  remove(filename.c_str());
}


TEST(NodeTest, NodeAddress1) {
  std::string filename = "./nodeoperatoreq.dat";
  NodeFile testnodefile(filename);
  Node node(&testnodefile, 1);
  // TODO: Fix what this is checking with a better understanding
  // Seems obvious that NODE 1 should go right after a FileHeader
  EXPECT_EQ(node.NodeAddress(), sizeof(FileHeader));
  node.CloseNode();
  remove(filename.c_str());
}


TEST(NodeTest, NodeAddress2) {
  std::string filename = "./nodeoperatoreq.dat";
  NodeFile testnodefile(filename);
  Node node(&testnodefile, 2);
  // TODO: Fix what this is checking with a better understanding
  // Meanwhile NODE 2 goes in after ONE NODE LENGTH OFFSET and the FileHeader
  EXPECT_EQ(node.NodeAddress(), nodelength + sizeof(FileHeader));
  node.CloseNode();
  remove(filename.c_str());
}
