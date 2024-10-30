#include <gtest/gtest.h>
#include "../src/node.h"

TEST(NodeTest, TestNodeConstructor) {
  std::string filename = "./constructor.dat";
  NodeFile myNodeFile(filename);
  bool newfile = access(filename.c_str(), 0) != 0;
  remove(filename.c_str());
  EXPECT_EQ(newfile, 0);
}


TEST(NodeTest, NodeFileCreateWriteOpen) {
  std::string filename = "./testfile.dat";
  try {
    NodeFile myNodeFile(filename);
    myNodeFile.header.deletednode = 1;
    myNodeFile.header.highestnode = 2;
    myNodeFile.WriteData(&(myNodeFile.header), sizeof myNodeFile.header, 0);
    // This NodeFile has an implicit ReadData
    NodeFile checkNode(filename);
    remove(filename.c_str());
    EXPECT_EQ(checkNode.header.deletednode, myNodeFile.header.deletednode);
    EXPECT_EQ(checkNode.header.highestnode, myNodeFile.header.highestnode);
  }catch (FileReadError) {
    remove(filename.c_str());
  }
}


TEST(NodeTest, DeleteFunctionNoChange) {
  std::string filename = "./deletefunctionnochange.dat";
  auto myNodeFile = new NodeFile (filename);
  delete myNodeFile;
  NodeFile checkNode(filename);
  remove(filename.c_str());
  EXPECT_EQ(checkNode.header.deletednode, myNodeFile->header.deletednode);
  EXPECT_EQ(checkNode.header.highestnode, myNodeFile->header.highestnode);
}

TEST(NodeTest, DeleteFunction) {
  std::string filename = "./deletefunctionchange.dat";
  try {
    auto myNodeFile = new NodeFile(filename);
    myNodeFile->header.deletednode = 1;
    myNodeFile->header.highestnode = 2;
    int myNodeFileHeaderDeleted = myNodeFile->header.deletednode;
    int myNodeFileHeaderHighest = myNodeFile->header.highestnode;
    delete myNodeFile;
    NodeFile checkNode(filename);
    //remove(filename.c_str());
    EXPECT_EQ(checkNode.header.deletednode, myNodeFileHeaderDeleted);
    EXPECT_EQ(checkNode.header.highestnode, myNodeFileHeaderHighest);
  }catch (FileReadError) {
    remove(filename.c_str());
  }
}

