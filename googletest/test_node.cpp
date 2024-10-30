#include <gtest/gtest.h>
#include "../src/node.h"

TEST(NodeTest, TestNodeConstructor) {
  char* filename = new char[16];
  strcpy(filename, "./testfile.txt");
  NodeFile myNodeFile(filename);
  bool newfile = access(filename, 0) != 0;
  remove(filename);
  EXPECT_EQ(newfile, 0);
}


TEST(NodeTest, NodeFileCreateWriteOpen) {
  int string_size = 16;
  std::string filename = "./testfile.txt";
  try {
    NodeFile myNodeFile(filename);
    myNodeFile.header.deletednode = 1;
    myNodeFile.header.highestnode = 2;
    myNodeFile.WriteData(&(myNodeFile.header), sizeof myNodeFile.header, 0);
    myNodeFile.nfile.close();
    // This NodeFile has an implicit ReadData
    NodeFile checkNode(filename);
    remove(filename.c_str());
    EXPECT_EQ(checkNode.header.deletednode, myNodeFile.header.deletednode);
  }catch (FileReadError) {
    remove(filename.c_str());
  }
}