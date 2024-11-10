#include "../src/parody.h"

#include <gtest/gtest.h>

// TEST(TestPersistent, Constructor) {}


#include <gtest/gtest.h>

class TestPersistent : public Persistent {
public:
    void Write();
    void Read();
    TestPersistent();
    explicit TestPersistent(Parody * pdb);
};

void TestPersistent::Write() {}
void TestPersistent::Read() {}

TestPersistent::TestPersistent() {
    changed = false;
    deleted = false;
    loaded = false;
}

TestPersistent::TestPersistent(Parody *pdb) {
    BuildObject();
}

class MockPersistentClass : public PersistentObject<int> {
public:
    explicit MockPersistentClass(int i) {
        Obj = i;
    }

};


TEST(TestParody, Constructor) {
    Parody pdb("testdatabase");
    EXPECT_EQ(pdb.rebuildnode, 0);
    // Ensure a fresh instance of the Parody database is created.
    EXPECT_EQ(Parody::opendatabase, &pdb);
}


TEST(TestParody, Destructor) {
    Parody* pdb = new Parody("testdatabase");
    delete pdb;
    // Ensure the database is closed and Parody::opendatabase resets.
    EXPECT_EQ(Parody::opendatabase, nullptr);
}


TEST(TestPersistent, Constructor) {
    Parody pdb("testdatabase");
    // Ensure the Persistent object initializes without error
    TestPersistent *persistent = new TestPersistent(&pdb);
}




/*
 *
 *I THINK -- Register a Class
 *Get a Class ID
 *Then create the class with the class name
 *THen you can add etc.
TEST(TestParody, RegisterIndexes) {
    Parody* pdb = new Parody("testdatabase");
    std::string mockclassname = "MockPersistentClass";
    Class mockClass((char *) mockclassname.c_str());
    NodeNbr nodeNum;
    MockPersistentClass mpc(1);
    //pdb->RegisterClass(mpc);
    // Ensure the database is closed and Parody::opendatabase resets.

}


 *TEST(TestParody, FindClassNoAdd) {
    Parody pdb("testdatabase");
    std::string mockclassname = "MockClassName";
    Class mockClass((char *) mockclassname.c_str());
    NodeNbr nodeNum;
    bool found = pdb.FindClass(&mockClass, &nodeNum);
    // Initial run may return false unless "MockClassName" exists
    //EXPECT_EQ(found, false);
}


TEST(TestParody, AddClassToIndex) {
    Parody pdb("testdatabase");
    Class mockClass("MockClassName");
    pdb.AddClassToIndex(&mockClass);
    //ClassID classID = pdb.GetClassID("MockClassName");
    //EXPECT_GT(classID, 0); // Class should now be indexed with a unique ID
}





TEST(TestParody, GetClassID) {
    Parody pdb("testdatabase");
    ClassID classID = pdb.GetClassID("NonExistentClass");
    // Without adding this class, it should have a default ID
    EXPECT_EQ(classID, 0); // Adjust based on mock behavior or initialization
}


TEST(TestParody, RegisterIndexes) {
    Parody pdb("testdatabase");
    TestPersistent mockPersistent;
    Class mockClass("MockClassName");
    pdb.RegisterIndexes(&mockClass, mockPersistent);
    // Ensure indexes were added; requires checking Parody internals
    EXPECT_TRUE(pdb.btrees.FirstEntry() != nullptr);
}

TEST(TestParody, RegisterClass) {
    Parody pdb("testdatabase");
    TestPersistent mockPersistent;
    ClassID classID = pdb.RegisterClass(mockPersistent);
    EXPECT_GT(classID, 0); // Should assign a positive ID for a new class
}


TEST(TestParody, GetObjectHeader) {
    Parody pdb("testdatabase");
    ObjectHeader objHeader{};
    ObjAddr testAddr = 123;
    pdb.GetObjectHeader(testAddr, objHeader);
    // Assuming the function reads correctly, verify data:
    EXPECT_EQ(objHeader.classid, 0); // Based on initial setup or mock
}


TEST(TestParody, Registration) {
    Parody pdb("testdatabase");
    TestPersistent pcls;
    Class* registeredClass = pdb.Registration(pcls);
    EXPECT_NE(registeredClass, nullptr);
}

*/


