// ------ btree.h

#ifndef BTREE_H
#define BTREE_H

#include <fstream>
#include <string>
#include "linklist.h"
#include "node.h"
#include "key.h"

class PdyBtree; // Forward declaration for PdyBtree
class TNode; // Forward declaration for PdyBtree
class IndexFile; // Forward declaration for IndexFile
//class Class; // Forward declaration for Class
class PdyKey; // Forward declaration for PdyKey


// ---- b-tree header record
class TreeHeader {
public:
    //friend class pdyBtree
    // friend class indexFile;
    NodeNbr rootnode; // node number of the root
    KeyLength keylength; // length of a key in this b-tree
    TreeHeader()
        { rootnode = 0; keylength = 0; }
};


// ----- b-tree index
class PdyBtree {
public:
    TreeHeader header;        // btree header
    TNode *trnode;            // -> current node value
    PdyKey *nullkey;          // for building empty derived key
    IndexFile &index;         // index file this tree lives on
    IndexNo indexno;          // 0= primary key, > 0= secondary key
    Class *classindexed;      // -> class structure of indexed class
    NodeNbr currnode;         // current node number
    NodeNbr oldcurrnode;      // for repositioning
    short oldcurrkey;         // for repositioning

    std::streampos HdrPos()
    { return classindexed->headeraddr + std::streamoff(indexno * sizeof(TreeHeader)); }
    void ReadHeader()
    { index.ReadData(&header, sizeof(TreeHeader), HdrPos());}
    void WriteHeader()
    { index.WriteData(&header, sizeof(TreeHeader), HdrPos());}
    void SaveKeyPosition();
    // public:
    PdyBtree(IndexFile &ndx, Class *cls, PdyKey *ky);
    // throw (BadKeylength);
    ~PdyBtree();
    void Insert(PdyKey *keypointer);
    void Delete(PdyKey *keypointer);
    bool Find(PdyKey *keypointer);
    PdyKey *Current();
    PdyKey *First();
    PdyKey *Last();
    PdyKey *Next();
    PdyKey *Previous();
    IndexFile& GetIndexFile() const
    { return index; }
    PdyKey *NullKey() const
    { return nullkey; }
    PdyKey *MakeKeyBuffer() const;
    NodeNbr Root() const
    { return header.rootnode; }
    NodeNbr GetKeyLength() const
    { return header.keylength; }
    IndexNo Indexno() const
    { return indexno; }
    const Class *ClassIndexed() const
    { return classindexed; }
    void SetClassIndexed (Class *cid)
    { classindexed = cid; }
};


// ------ b-tree TNode class
class TNode : public Node {
public:
    // friend class PdyBtree;
    struct TNodeHeader {
        bool isleaf;
        NodeNbr parent;
        NodeNbr leftsibling;
        NodeNbr rightsibling;
        short int keycount;
        NodeNbr lowernode;

        TNodeHeader()
        { isleaf = false; parent = leftsibling = rightsibling = keycount = lowernode = 0;}

    } header;
public:
    PdyKey *currkey;
    PdyBtree *btree;
    LinkedList<PdyKey> keys;

    TNode(PdyBtree *bt, NodeNbr node);
    bool SearchNode(PdyKey *keyvalue);
    void Insert(PdyKey *keyvalue);
    short int m();
    void WriteBtreeKey(PdyKey *thiskey);
    void Adopt(NodeNbr node);
    void Adoption();
    bool isLeaf() const
        { return header.isleaf; }
    NodeNbr Parent() const
        { return header.parent; }
    NodeNbr LeftSibling() const
        { return header.leftsibling; }
    NodeNbr RightSibling() const
        { return header.rightsibling; }
    short int KeyCount() const
        { return header.keycount; }
    NodeNbr LowerNode() const
        { return header.lowernode; }
    bool Redistribute(NodeNbr sib);
    bool Implode(TNode &right);
    short int NodeHeaderSize() const
        { return sizeof(TNodeHeader) + Node::NodeHeaderSize();}
    TNode& operator=(TNode& tnode);
    ~TNode();
};

#endif