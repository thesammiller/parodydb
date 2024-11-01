// ------ btree.cpp

#include "string.h"
#include "btree.h"

// ------ constructor to open a btree
PdyBtree::PdyBtree(IndexFile &ndx, Class *cls, PdyKey *ky)
                        /// throw (BadKeyLength)
                        : index(ndx)
{
    nullkey = ky->MakeKey();
    nullkey->PdyKey::operator=(*ky);
    trnode = 0;
    classindexed = cls;
    currnode = 0;

    indexno = ky->indexno;

    // ------ read the btree header
    ReadHeader();

    if (header.keylength == 0)
        { header.keylength = ky->keylength; }
    else {
        if (ky->keylength != 0 && header.keylength != ky->keylength) {
            //throw BadKeyLength()
            // TODO: Error handling
        }
    }
}

// ------ destructor for a btree
PdyBtree::~PdyBtree()
{
    // ---- write the btree header
    WriteHeader();
    delete trnode;
    delete nullkey;
}

// ------ make a key buffer
PdyKey *PdyBtree::MakeKeyBuffer() const
{
    PdyKey *thiskey = nullkey->MakeKey();
    thiskey->indexno = indexno;
    return thiskey;
}

// ------ insert a key into a btree
void PdyBtree::Insert(PdyKey *keypointer)
{
    // don't insert duplicate key
    if (!Find(keypointer)) {
        PdyKey *newkey = keypointer->MakeKey();
        *newkey = *keypointer;

        NodeNbr rootnode = 0;
        NodeNbr leftnode = 0;
        NodeNbr rightnode = 0;
        bool RootIsLeaf = true;

        bool done = false;
        // ------ insert key into btree
        while (currnode) {
            int em = trnode->m();
            trnode->Insert(newkey);
            // first insertion is into leaf
            // if split later insertions
            // are into parents (non-leaves)
            if (!trnode->header.isleaf)
                { trnode->currkey->lowernode = rightnode; }
            done = trnode->header.keycount <= em;
            if (!done) {
                // ---- node is full
                //     try to redistribute keys among siblings
                done = trnode->Redistribute(trnode->header.leftsibling);
            }
            if (!done) {
                done = trnode->Redistribute(trnode->header.rightsibling);
            }
            if(done)
                { break; }
            // ------ cannot redistribute filled node, split it
            RootIsLeaf = false;

            rightnode = index.NewNode();
            leftnode = currnode;

            TNode right(this, rightnode);
            right.SetNodeNbr(rightnode);
            right.MarkNodeChanged();

            // ---- establish sibling and parent relationships
            //    between current node and new sibling
            right.header.rightsibling = trnode->header.rightsibling;
            trnode->header.rightsibling = rightnode;
            right.header.leftsibling = currnode;
            right.header.parent = trnode->header.parent;

            // ------ if the current node is a leaf,
            //     so is the new sibling
            right.header.isleaf = trnode->header.isleaf;

            // ----- compute new key counts for the two nodes
            trnode->header.keycount = (em + 1) / 2;
            right.header.keycount = em-trnode->header.keycount;

            // ------ locate the middle key in the current node
            PdyKey *middlekey = trnode->keys.FindEntry(trnode->header.keycount);

            // ------ set the pointer to keys less than
            //        those in new node
            if (!right.header.isleaf) {
                right.header.lowernode = middlekey->lowernode;
            }

            // ------ point to the keys to move (1 past middle)
            PdyKey *movekey = trnode->keys.NextEntry(middlekey);

            // ------ middle key inserts into parent
            trnode->keys.RemoveEntry(middlekey);
            *newkey = *middlekey;
            delete middlekey;

            // ------ move keys from current to new right node
            for (int i = 0; i < right.header.keycount; i++) {
                PdyKey *nkey = trnode->keys.NextEntry(movekey);
                trnode->keys.RemoveEntry(movekey);
                right.keys.AppendEntry(movekey);
                movekey = nkey;
            }

            // ---- prepare to insert key
            //      into parent of split nodes
            currnode = trnode->header.parent;
            if (!currnode) {
                // ---- no parent node, splitting the root node
                rootnode = index.NewNode();
                right.header.parent = rootnode;
                trnode->header.parent = rootnode;
            }

            // ---- the former right sibling of the curent node
            //    is now the right sibling of the split node
            //     and must record the new node as left sibling
            if (right.header.rightsibling) {
                TNode farright(this, right.header.rightsibling);
                farright.header.leftsibling = rightnode;
                farright.MarkNodeChanged();
            }

            // ---- children of the new split node point to
            //      the current split node as parent
            if (!right.header.isleaf) {
                right.Adoption();
            }

            // ---- if splitting other than root, read parent
            //     position currkey to key where split node
            //    key will be inserted
            if (currnode) {
                delete trnode; // writes the split node to disk
                // ---- get the parent of the split nodes
                trnode = new TNode(this, currnode);
                // ---- position currkey where new key will insert
                trnode->SearchNode(newkey);
            }
        }
        if (!done) {
            // ---- mew rppt mpde ==== (
            delete trnode;
            if (rootnode == 0) {
                rootnode = index.NewNode();
            }
            trnode = new TNode(this, rootnode);
            trnode->header.isleaf = RootIsLeaf;
            currnode = header.rootnode = rootnode;
            trnode->SetNodeNbr(rootnode);
            trnode->Insert(newkey);
            trnode->header.parent = 0;
            trnode->header.keycount = 1;
            if (!RootIsLeaf) {
                trnode->header.lowernode = leftnode;
                trnode->currkey->lowernode = rightnode;
            }
            trnode->MarkNodeChanged();
        }
        delete newkey;
    }
    delete trnode;
    trnode = 0;
}

void PdyBtree::SaveKeyPosition() {
    if (trnode->header.isleaf) {
        oldcurrnode = 0;
        oldcurrkey = 0;
    }
    else {
        oldcurrnode = currnode;
        oldcurrkey = trnode->keys.FindEntry(trnode->currkey);
    }
}

// ------ find a key in a btree
bool PdyBtree::Find(PdyKey *keypointer)
{
    oldcurrnode = 0;
    oldcurrkey = 0;

    currnode = header.rootnode;
    while (currnode) {
        delete trnode;
        trnode = new TNode(this, currnode);

        if (trnode->SearchNode(keypointer)) {
            // ----- search key is equal to a key in the node
            keypointer->fileaddr = trnode->currkey->fileaddr;
            oldcurrnode = 0;
            oldcurrkey = 0;
            return true;
        }

        if (trnode->currkey == trnode->keys.FirstEntry()) {
            // ---- search key is < lowest key in node
            SaveKeyPosition();
            if (trnode->header.isleaf)
                { break; }
            currnode = trnode->keys.PrevEntry(trnode->currkey)->lowernode;
        }
        else {
            // ---- search key > highest key in node
            if (trnode->header.isleaf) {
                break;
            }
            currnode = trnode->keys.LastEntry()->lowernode;
        }
    }
    return false;
}

// ------ delete a key form a btree
void PdyBtree::Delete(PdyKey *keypointer)
{
    if (Find(keypointer)) {
        if (!trnode->header.isleaf) {
            // ---- if not found in leaf node, go down to leaf
            TNode *leaf = new TNode(this, trnode->currkey->lowernode);
            while (!leaf->header.isleaf) {
                NodeNbr lf = leaf->header.lowernode;
                delete leaf;
                leaf = new TNode(this, lf);
            }

            // ---- Move the left-most key from the leaf
            //     to where deleted key was in higher node
            PdyKey *movekey = leaf->keys.FirstEntry();
            leaf->keys.RemoveEntry(movekey);
            leaf->header.keycount--;
            leaf->MarkNodeChanged();

            trnode->keys.InsertEntry(movekey, trnode->currkey);

            movekey->lowernode = trnode->currkey->lowernode;

            trnode->keys.RemoveEntry(trnode->currkey);
            delete trnode->currkey;
            trnode->MarkNodeChanged();
            delete trnode;

            trnode = leaf;
            trnode->currkey = trnode->keys.FirstEntry();
            currnode = trnode->GetNodeNbr();
        }
        else {
            // ---- delete the key from the node
            trnode->keys.RemoveEntry(trnode->currkey);
            delete trnode->currkey;
            trnode->header.keycount--;
            trnode->MarkNodeChanged();
            if (trnode->header.keycount == 0) {
                header.rootnode=0;
            }
        }
        // ---- if the node shrinks to half capacity
        //    try to combine it with a sibling node
        while (trnode->header.keycount > 0 && trnode->header.keycount <=trnode->m()/2) {
            if (trnode->header.rightsibling) {
                TNode *right = new TNode(this, trnode->header.rightsibling);
                if (trnode->Implode(*right)) {
                    delete right;
                    NodeNbr parent = trnode->header.parent;
                    if (parent == 0) {
                        header.rootnode = trnode->GetNodeNbr();
                        break;
                    }
                    delete trnode;
                    trnode = new TNode(this, parent);
                    continue;
                }
                delete right;
            }
            if (trnode->header.leftsibling) {
                TNode *left = new TNode(this, trnode->header.leftsibling);
                if (left->Implode(*trnode)) {
                    delete trnode;
                    NodeNbr parent = left->header.parent;
                    if (parent == 0) {
                        header.rootnode = left->GetNodeNbr();
                        trnode = left;
                        break;
                    }
                    delete left;
                    trnode = new TNode(this, parent);
                    continue;
                }
                delete left;
            }

            // --- could not combine with either sibling
            //     try to redistribute
            if (!trnode->Redistribute(trnode->header.leftsibling)) {
                trnode->Redistribute(trnode->header.rightsibling);
            }
            break;
        }
    }
    delete trnode;
    trnode = 0;
}

// ------ return the address of the current key
PdyKey *PdyBtree::Current()
{
    if (trnode == 0) {
        return 0;
    }
    // oldcurrnode != 0 after SaveKeyPosition()
    // Restores previous node as current node
    if (oldcurrnode != 0) {
        currnode = oldcurrnode;
        delete trnode;
        trnode = new TNode(this, currnode);
        trnode->currkey = trnode->keys.FindEntry(oldcurrkey);
        oldcurrnode = 0;
        oldcurrkey = 0;
    }
    return trnode->currkey;
}

// ------ return the address of the first key
PdyKey *PdyBtree::First()
{
    currnode = header.rootnode;
    if (currnode) {
        delete trnode;
        trnode = new TNode(this, currnode);
        while (!trnode->header.isleaf) {
            currnode = trnode->header.lowernode;
            delete trnode;
            trnode = new TNode(this, currnode);
        }
        trnode->currkey = trnode->keys.FirstEntry();
    }
    return Current();
}

// ------ return the address of the last key
PdyKey *PdyBtree::Last()
{
    currnode = header.rootnode;
    if (currnode) {
        delete trnode;
        trnode = new TNode(this, currnode);
        while (!trnode->header.isleaf) {
            currnode = trnode->keys.LastEntry()->lowernode;
            delete trnode;
            trnode = new TNode(this, currnode);
        }
        trnode->currkey = trnode->keys.LastEntry();
    }
    return Current();
}

// ------ return the address of the next key
PdyKey *PdyBtree::Next()
{
    if (trnode == 0 || trnode->currkey == 0) {
        return First();
    }
    if (!trnode->header.isleaf) {
        // ---- current key is not in a leaf
        currnode = trnode->currkey->lowernode;
        delete trnode;
        trnode = new TNode(this, currnode);
        // ---- go down to the leaf;
        while (!trnode->header.isleaf) {
            currnode = trnode->header.lowernode;
            delete trnode;
            trnode = new TNode(this, currnode);
        }
        // ---- use the first key in the leaf as the next one
        trnode->currkey = trnode->keys.FirstEntry();
    }
    else {
        // ---- current key is in a leaf
        PdyKey *thiskey = nullkey->MakeKey();
        *thiskey = *(trnode->currkey);

        // --- point to the next key in the leaf
        trnode->currkey = trnode->keys.NextEntry(trnode->currkey);
        while(trnode->currkey == 0 && currnode != header.rootnode) {
            TNode pnode(this, trnode->Parent());
            pnode.SearchNode(thiskey);
            currnode = pnode.GetNodeNbr();
            *trnode = pnode;
        }
        delete thiskey;
    }
    return Current();
}

// -- return the address of the previous key
PdyKey *PdyBtree::Previous()
{
    if (trnode == 0 || trnode->currkey == 0) {
        return Last();
    }
    if (!trnode->header.isleaf) {
        // ---- current key is not in a leaf
        PdyKey *ky = trnode->keys.PrevEntry(trnode->currkey);
        if (ky != 0) {
            currnode = ky->lowernode;
        }
        else {
            currnode = trnode->header.lowernode;
        }
        delete trnode;
        trnode = new TNode(this, currnode);
        // ------ go down to the leaf
        while (!trnode->header.isleaf) {
            currnode = trnode->keys.LastEntry()->lowernode;
            delete trnode;
            trnode = new TNode(this, currnode);
        }
        // ---- use the last key in the leaf as the next one
        trnode->currkey = trnode->keys.LastEntry();
    }
    else {
        // ------ current key is in a leaf
        PdyKey *thiskey = nullkey->MakeKey();
        *thiskey = *(trnode->currkey);

        // ---- point to the previous key in the leaf
        trnode->currkey = trnode->keys.PrevEntry(trnode->currkey);
        while (trnode->currkey == 0 && currnode != header.rootnode) {
            // --- current key was the first one in the leaf
            TNode pnode(this, trnode->Parent());
            pnode.SearchNode(thiskey);

            if (pnode.currkey == 0) {
                pnode.currkey = pnode.keys.LastEntry();
            }
            else {
                pnode.currkey = pnode.keys.PrevEntry(pnode.currkey);
            }
            currnode = pnode.GetNodeNbr();
            *trnode = pnode;
        }
        delete thiskey;
    }
    return Current();
}
