// ------ key.cpp

#include "key.h"
// Uses IndexFile, which is derived from NodeFile
#include "node.h"



// ======
// base PdyKey class member functions
// ====
PdyKey::PdyKey(NodeNbr fa)
{
    fileaddr = fa;
    lowernode = 0;
    indexno = 0;
    relatedclass = 0;
    /* TODO: Persistent being called here seems weird to me
     * Need to do more research about how this is setup
     * Ideally would like to remove circular dependencies

    if (Persistent::objconstructed != 0) {
        // ---- register the key with the object being built
        Persistent::objconstructed->RegisterKey(this);
        //TODO: Seems like assignment here is important
        // ---- assign index number based on position in object
        indexno = Persistent::objconstructed->indexcount++;
    }
     */
}

// ------ overloaded =
PdyKey& PdyKey::operator=(const PdyKey& key)
{
    if (this != &key) {
        fileaddr = key.fileaddr;
        lowernode = key.lowernode;
        indexno = key.indexno;
        keylength = key.keylength;
        relatedclass = key.relatedclass;
    }
    return *this;
}


