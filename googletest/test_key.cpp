//
// Created by Sam Miller on 10/29/24.
//
#include <gtest/gtest.h>
#include "../src/key.h"

// Can't test PdyKey directly since it's abstract
//====
// TestPdyKey
// ====
class TestPdyKey : public PdyKey {
public:
  virtual void WriteKey(IndexFile &bfile) {};
  virtual void ReadKey(IndexFile &bfile) {};
  bool isNullValue(IndexFile &bfile);
  void CopyKeyData(const PdyKey &key) {}
  bool isObjectAddress() const
    { return false; }
  const ObjAddr *ObjectAddress() const
    { return 0; }
  PdyKey *MakeKey() const { return new TestPdyKey(1); };

  PdyKey &operator=(const PdyKey &key) { return *this; };
  TestPdyKey(const NodeNbr &nbr)
    {lowernode = nbr; fileaddr = 0; };
  ~TestPdyKey() {};
  int operator>(const PdyKey &key) const { return false; }
  int operator==(const PdyKey &key) const { return false; }
};

bool TestPdyKey::isNullValue(IndexFile &bfile) {
  return true;
}

// struct Class
TEST(PdyKeyTest, PdyConstructor) {
  const NodeNbr nodenbr = 1;
  TestPdyKey pdykey(nodenbr);
  EXPECT_EQ(pdykey.lowernode, nodenbr);
}

TEST(PdyKeyTest, PdyKeyOperatorEq) {
  const NodeNbr nodenbr1 = 1;
  const NodeNbr nodenbr2 = 2;
  TestPdyKey pdykey1(nodenbr1);
  TestPdyKey pdykey2(nodenbr2);
  pdykey1.fileaddr = nodenbr1;
  pdykey1.lowernode = nodenbr1;
  pdykey1.indexno = nodenbr1;
  pdykey1.keylength = nodenbr1;
  pdykey2 = pdykey1;
  EXPECT_EQ(pdykey1.fileaddr, pdykey2.fileaddr);
  EXPECT_EQ(pdykey1.lowernode, pdykey2.lowernode);
  EXPECT_EQ(pdykey1.indexno, pdykey2.indexno);
  EXPECT_EQ(pdykey1.keylength, pdykey2.keylength);
}

TEST(KeyTest, KeyConstructor) {
  int value = 1;
  Key<int> key(value);
  EXPECT_EQ(key.KeyValue(), value);
  EXPECT_EQ(key.keylength, sizeof(int));
}

TEST(KeyTest, KeyCopyKeyData) {
  int value1 = 1;
  int value2 = 2;
  Key<int> key1(value1);
  Key<int> key2(value2);
  key1.CopyKeyData(key2);
  EXPECT_EQ(key1.KeyValue(), value2);
}

TEST(KeyTest, KeyOperatorEq) {
  int value1 = 1;
  int value2 = 2;
  Key<int> key1(value1);
  Key<int> key2(value2);
  key1 = key2;
  EXPECT_EQ(key1.KeyValue(), value2);
}

TEST(KeyTest, KeyOperatorGt) {
  int value1 = 1;
  int value2 = 2;
  Key<int> key1(value1);
  Key<int> key2(value2);
  bool result1 = key1 > key2;
  bool result2 = key2 > key1;
  EXPECT_EQ(result1, false);
  EXPECT_EQ(result2, true);
}


TEST(KeyTest, KeyOperatorEqq) {
  int value1 = 1;
  int value3 = 3;
  Key<int> key1(value1);
  Key<int> key2(value1);
  Key<int> key3(value3);
  bool result1 = key1 == key2;
  bool result2 = key2 == key3;
  EXPECT_EQ(result1, true);
  EXPECT_EQ(result2, false);
}

// TODO: Is this what MakeKey is doing?
TEST(KeyTest, MakeKey) {
  int value1 = 1;
  Key<int> key1(value1);
  auto newkey = key1.MakeKey();
  EXPECT_EQ(newkey->keylength, key1.keylength);
}

TEST(KeyTest, KeyReadWrite) {
  int value = 5;
  std::string filename = "./keytest";
  IndexFile idx_file(filename);
  Key<int> key(value);
  key.WriteKey(idx_file);
  IndexFile read_file(filename+".idx");
  key.ReadKey(read_file);
  remove((filename+".idx").c_str());
  EXPECT_EQ(key.KeyValue(), value);
}








