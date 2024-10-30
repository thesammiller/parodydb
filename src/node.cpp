// ------ node.cpp

// ======
// Parody Node and NodeFile class member functions
// ======

#include <unistd.h>
#include "node.h"

NodeFile::NodeFile(const std::string& filename) throw (BadFileOpen)
{
    header = FileHeader();
    newfile = access(filename.c_str(), 0) != 0;
    // ---- open the file
    // ChatGPT fix: When opening a file with both std::ios::in and std::ios::out without std::ios::trunc or std::ios::app, the file must already exist; otherwise, fstream::open will fail.
    if (!newfile) {
        nfile.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
        if (nfile.fail())
        {
            throw BadFileOpen();
        }
        // an existing file, read the header
        ReadData(&header, sizeof header);
    }
    else {
        nfile.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        if (nfile.fail())
        {
            throw BadFileOpen();
        }
        // ------ creating the file, write the empty header
        WriteData(&header, sizeof header);
    }
    origheader = header;
}

NodeFile::~NodeFile()
{
    if (header.deletednode != origheader.deletednode || header.highestnode != origheader.highestnode) {
        // ------ the file header has changed
        WriteData(&header, sizeof header, 0);
    }
    nfile.close();
}

void NodeFile::ReadData(void *buf, unsigned short siz, long wh) throw (FileReadError)
{
    if (wh != -1) {
        nfile.seekg(wh);
    }
    nfile.read(reinterpret_cast<char*>(buf), siz);
    if (nfile.fail() || nfile.eof()) {
        nfile.clear();
        throw FileReadError();
    }
    nfile.seekp(nfile.tellg());
}

void NodeFile::WriteData(const void *buf, unsigned short siz, long wh) throw (FileWriteError)
{
    if (wh != -1) {
        nfile.seekp(wh);
    }
    nfile.write(reinterpret_cast<const char*>(buf), siz);

    if (nfile.fail()) {
        nfile.clear();
        throw FileWriteError();
    }
    nfile.seekg(nfile.tellp());
}

// ---- appropriate a new node
NodeNbr NodeFile::NewNode()
{
    NodeNbr newnode;
    if (header.deletednode) {
        newnode = header.deletednode;
        Node node(this, newnode);
        header.deletednode = node.NextNode();
        node.SetNextNode(0);
    }
    else {
        newnode = ++header.highestnode;
    }
    return newnode;
}

// ------ construct a new node
Node::Node(NodeFile *hd, NodeNbr node)
{
    nextnode = 0;
    nodechanged = deletenode = false;
    nodenbr = node;
    owner = hd;
    if (nodenbr) {
        long nad = NodeAddress();
        // ------ read the header
        try {
            owner->ReadData(&nextnode, sizeof nextnode, nad);
        }
        catch (FileReadError) {
            owner->WriteData(&nextnode, sizeof nextnode, nad);
        }
    }
}

// ----- close a node
void Node::CloseNode()
{
    if (owner && nodenbr && (nodechanged || deletenode)) {
        if (deletenode) {
            nextnode = owner->DeletedNode();
            owner->SetDeletedNode(nodenbr);
        }
        long nad = NodeAddress();
        // ------ write the header
        owner->WriteData(&nextnode, sizeof nextnode, nad);
        if (deletenode) {
            // ---- zero fill the deleted node
            char fill[nodedatalength];
            memset(fill, 0, nodedatalength);
            fill[0] = -1; // mark the node deleted
            owner->WriteData(fill, nodedatalength);
        }
    }
}

// ----- assignment operator
Node &Node::operator=(const Node &node)
{
    CloseNode();
    nextnode = node.nextnode;
    owner = node.owner;
    nodenbr = node.nodenbr;
    nodechanged = node.nodechanged;
    deletenode = node.deletenode;
    return *this;
}

// ------ destroy the node
Node::~Node()
{
    CloseNode();
}

// ------ compute the disk address of a node
long Node::NodeAddress()
{
    // TODO: Added to prevent negative address
    // May want to remove this later
    // TODO: Is nodenbr  1 indexed, or 0 indexed?
    if (nodenbr == 0) {
        return 0;
    }
    long adr = nodenbr - 1;
    adr *= nodelength;
    // This gives a starting head of 4 when nodenbr = 1
    adr += sizeof(FileHeader);
    return adr;
}