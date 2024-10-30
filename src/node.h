// ----- node.h

#include <fstream>

#ifndef NODE_H
#define NODE_H

#include "structs.h"


const short int nodelength = 128;
const short int nodedatalength = nodelength - sizeof(NodeNbr);

// ----- exceptions to be thrown
class BadFileOpen : public std::exception {};
class FileReadError: public std::exception {};
class FileWriteError: public std::exception {};

// ====
// FileHeader
// -- NodeNbr deletednode
// -- Nodenbr highestnode
// ====
class FileHeader {
public:
	NodeNbr deletednode; // first deleted node
	NodeNbr highestnode;
    // friend class NodeFile
	FileHeader() { deletednode = highestnode = 0; }
};

// ====
// NodeFile class
// -- FileHeader header;
// -- fstream nfile
// -- NodeFile(std::string& filename)
// ====
class NodeFile {
public:
    FileHeader header;
    FileHeader origheader;
    std::fstream nfile;
    bool newfile;
// public:
    NodeFile(const std::string& filename)
        throw (BadFileOpen);
    virtual ~NodeFile();
    void SetDeletedNode(NodeNbr node)
            { header.deletednode = node; }
    NodeNbr DeletedNode() const
            { return header.deletednode; }
    void SetHighestNode(NodeNbr node)
            { header.highestnode = node; }
    NodeNbr HighestNode() const
            { return header.highestnode; }
    NodeNbr NewNode();
    void ReadData(void *buf, unsigned short siz, long wh = -1) throw (FileReadError);
    void WriteData(const void *buf, unsigned short siz, long wh = -1) throw (FileWriteError);
    void Seek(std::streampos offset, std::ios::seekdir dir = std::ios::beg)
            { nfile.seekg(offset, dir); nfile.seekp(offset, dir);}
    std::streampos FilePosition()
        { return nfile.tellg(); }
    bool NewFile() const
        { return newfile; }
    void ResetNewFile()
        { newfile = false; }
};

// ======
// Node Record
// ======
class Node {
public:
    NodeNbr nextnode;
    void CloseNode();
    NodeFile *owner;
    NodeNbr nodenbr;
    bool nodechanged;
    bool deletenode;
public:
    Node(NodeFile *nd = 0, NodeNbr node = 0);
    virtual ~Node();
    Node &operator=(const Node& node);
    void SetNextNode(NodeNbr node)
        { nextnode = node; MarkNodeChanged(); }
    NodeNbr NextNode() const
        { return nextnode; }
    void SetNodeNbr(NodeNbr node)
        { nodenbr = node; }
    NodeNbr GetNodeNbr() const
        { return nodenbr; }
    void MarkNodeDeleted()
        { deletenode = true; }
    void MarkNodeChanged()
        { nodechanged = true; }
    bool NodeChanged() const
        { return nodechanged; }
    long NodeAddress();
    virtual short int NodeHeaderSize() const
        { return sizeof(NodeNbr); }
};


// ======
// IndexFile Class
// ======

class IndexFile : public NodeFile {
public:
    IndexFile(const std::string& name) : NodeFile(name+".ndx")
    {/* ... */}
};

#endif
	
