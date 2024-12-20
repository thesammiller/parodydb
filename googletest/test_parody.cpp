#include <gtest/gtest.h>
#include "../src/parody.h"

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
    Persistent *persistent = new Persistent(pdb);
    EXPECT_TRUE(pdb.OpenDatabase() != 0);
}


TEST(TestPersistent, Registration) {
    Parody pdb("testdatabase");
    // Ensure the Persistent object initializes without error
    Persistent *persistent = new Persistent(pdb);
    EXPECT_TRUE(pdb.OpenDatabase() != 0);
    auto cls = pdb.Registration(*persistent);
    // 0
    EXPECT_EQ(cls, pdb.classes.FirstEntry());
}

/*
TEST(TestPersistentObject, Constructor) {
    Parody pdb("testdatabase");
    auto hello = std::string("hello");
    printf("1 -- Step\n");
    PersistentObject<std::string> mpc(hello);
    printf("2 -- Step\n");
}*/

class SSN {
public:
    long ssn;
    SSN(long s = 0) : ssn(s) {};
};

const short NameLength = 15;

struct PayrollRcd {
    SSN ssn;
    char name[NameLength+1];
    void Header();
};

TEST(Payroll, AddRecord) {
    //main
    auto *payroll = new Parody("PAYROLL");
    PayrollRcd pr;
    SSN ssn = SSN(1234567890);
    pr.ssn = ssn;
    strcpy(pr.name, "John");
    PersistentObject<PayrollRcd> ppr(pr);
    ppr.AddObject();
    ppr.SaveObject();
    delete payroll;
    remove("PAYROLL.ndx");
    remove("PAYROLL.dat");
}


TEST(Payroll, SaveObject) {
    remove("PAYROLL.dat");
    remove("PAYROLL.ndx");
}


TEST(Payroll, FirstObject) {
    //main
    auto *payroll = new Parody("PAYROLL");
    PayrollRcd pr;
    SSN ssn = SSN(1234567890);
    pr.ssn = ssn;
    strcpy(pr.name, "John");
    PersistentObject<PayrollRcd> ppr(pr);
    ppr.AddObject();
    ppr.SaveObject();
    PersistentObject<PayrollRcd> openpr;
    openpr.FirstObject();
    EXPECT_STREQ(openpr.Obj.name, pr.name);
    EXPECT_EQ(openpr.Obj.ssn.ssn, pr.ssn.ssn);
    delete payroll;
    remove("PAYROLL.dat");
    remove("PAYROLL.ndx");
}

TEST(Payroll, NextObject) {
    //main
    auto *payroll = new Parody("PAYROLL");
    PayrollRcd pr;
    SSN ssn = SSN(1234567890);
    pr.ssn = ssn;
    strcpy(pr.name, "John");
    PersistentObject<PayrollRcd> ppr(pr);
    ppr.AddObject();
    ppr.SaveObject();
    PayrollRcd kermit;
    SSN kermitssn = SSN(333445555);
    kermit.ssn = kermitssn;
    strcpy(kermit.name, "Kermit");
    PersistentObject<PayrollRcd> kermitpr(kermit);
    kermitpr.AddObject();
    kermitpr.SaveObject();
    PersistentObject<PayrollRcd> openpr;
    openpr.FirstObject();
    openpr.NextObject();
    EXPECT_STREQ(openpr.Obj.name, kermit.name);
    EXPECT_EQ(openpr.Obj.ssn.ssn, kermit.ssn.ssn);
    delete payroll;
    remove("PAYROLL.dat");
    remove("PAYROLL.ndx");
}


TEST(Payroll, FindIndexNotZero) {
    //main
    auto *payroll = new Parody("PAYROLL");
    PayrollRcd pr;
    SSN ssn = SSN(1234567890);
    pr.ssn = ssn;
    strcpy(pr.name, "John");
    PersistentObject<PayrollRcd> ppr(pr);
    ppr.AddObject();
    ppr.SaveObject();
    PayrollRcd kermit;
    SSN kermitssn = SSN(333445555);
    kermit.ssn = ssn;
    strcpy(kermit.name, "Kermit");
    PersistentObject<PayrollRcd> kermitpr(kermit);
    kermitpr.AddObject();
    kermitpr.SaveObject();
    PersistentObject<PayrollRcd> openpr;
    openpr.FirstObject();
    //auto key = new PdyKey(1);
    openpr.FindIndex(0);
    EXPECT_STREQ(openpr.Obj.name, pr.name);
    EXPECT_EQ(openpr.Obj.ssn.ssn, pr.ssn.ssn);
    delete payroll;
    remove("PAYROLL.dat");
    remove("PAYROLL.ndx");
}

// This is the basic function of a persistent database...
// Opening and closing a persistent file
TEST(Payroll, OpenAndClose) {
    //main
    auto *payroll = new Parody("PAYROLL");
    PayrollRcd pr;
    SSN ssn = SSN(1234567890);
    pr.ssn = ssn;
    int namelength = 4;
    strncpy(pr.name, "John", namelength);
    pr.name[namelength] = '\0';
    PersistentObject<PayrollRcd> ppr(pr);
    ppr.AddObject();
    ppr.SaveObject();
    delete payroll;

    auto *payroll2 = new Parody("PAYROLL");
    PersistentObject<PayrollRcd> openpr;
    openpr.FirstObject();
    EXPECT_TRUE(openpr.ObjectExists());
    EXPECT_STREQ(openpr.Obj.name, pr.name);
    EXPECT_EQ(openpr.Obj.ssn.ssn, pr.ssn.ssn);
    delete payroll2;
    //remove("PAYROLL.dat");
    //remove("PAYROLL.ndx");
}

