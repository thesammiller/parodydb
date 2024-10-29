#ifndef STRUCTS_H
#define STRUCTS_H

#include <fstream>

typedef unsigned short int NodeNbr;

typedef short int IndexNo;

typedef short int ClassID;
const int classnamesize = 32;

typedef short int KeyLength;

//Class
struct Class {
    char *classname;
    ClassID classid;
    std::streampos headeraddr;

    // Constructor
    Class(char *cn = 0) : classname(cn), classid(0), headeraddr(0)
        { /*...*/ }
};

// ====
// object addr
// ====
struct ObjAddr {
    NodeNbr oa;

    // Constructor
    ObjAddr(NodeNbr nd = 0) : oa(nd)
        { /* ... */ }

    // Implicit conversion
    // e.g. NodeNbr x = myobjaddr
    operator NodeNbr() const
        { return oa; }
};

// ====
// persistent object header rcd
// ====
struct ObjectHeader {
    ClassID classid; // class id
    NodeNbr ndnbr; // relative node number within object

    // Constructor
    ObjectHeader() : classid(0), ndnbr(0)
        { /* ... */ }
};


#endif