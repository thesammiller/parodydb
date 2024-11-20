// ------ parody.cpp

#include "parody.h"
#include "string.h"

Parody *Parody::opendatabase; // latest open database

// ======
// Parody Member functions
// ======

// ----- construct a Parody database
Parody::Parody(const std::string &name) : datafile(name), indexfile(name)
{
	rebuildnode = 0;
	previousdatabase = opendatabase;
	opendatabase = this;
}

// ------ close the Parody database
Parody::~Parody()
{
	PdyBtree *bt = btrees.FirstEntry();
	while (bt != 0) {
		delete bt;
	bt = btrees.NextEntry();
	}
	Class *cls = classes.FirstEntry();
	while (cls != 0) {
		delete [] cls->classname;
	delete cls;
	cls = classes.NextEntry();
	}
	opendatabase = previousdatabase;
}

// ------ read an object header record
void Parody::GetObjectHeader(ObjAddr nd, ObjectHeader &objhdr)
{
	// --- constructing this node seeks to the first data byte
	Node(&datafile, nd);
	datafile.ReadData(&objhdr, sizeof(ObjectHeader));
}

Class *Parody::Registration(const Persistent& pcls)
{
	Class *cls = classes.FirstEntry();
	while (cls != 0) {
		const char *ty = typeid(pcls).name();
		if (strcmp(cls->classname, ty) == 0) {
			break;
		}
		cls = classes.NextEntry();
	}
	return cls;
}

bool Parody::FindClass(Class *cls, NodeNbr *nd)
{
	char classname[classnamesize];
	ClassID cid = 0;
    // If indexfile is not new
    if (!indexfile.NewFile()) {
        Node tmpnode;
		NodeNbr nx = 1;
		// locate class header
        // added second part
		while (nx != 0) {
			tmpnode = Node(&indexfile, nx);
		    indexfile.ReadData(classname, classnamesize);
		    if (strcmp(classname, cls->classname) == 0) {
		        cls->headeraddr = indexfile.FilePosition();
		        cls->classid = cid;
		        return true;
		    }
			// ---- this node is not hte class header
			cid++;
			nx = tmpnode.NextNode();
		}
		if (nd != 0) {
			*nd = indexfile.NewNode();
			tmpnode.SetNextNode(*nd);
		}
	}
	cls->classid = cid;

	return false;
}

ClassID Parody::GetClassID(const char *classname)
{
	Class cls(const_cast<char*>(classname));
	FindClass(&cls);
	return cls.classid;
}

void Parody::AddClassToIndex(Class *cls)
{
	NodeNbr nd = 0;
	if (FindClass(cls, &nd) == false) {
		indexfile.ResetNewFile();
		nd = nd ? nd : indexfile.NewNode();
		// ---- build the class header for new class
		Node tmpnode(&indexfile, nd);

		// ---- write class name into class record
		indexfile.WriteData(cls->classname, classnamesize);

		// ---- save disk address of tree headers
		cls->headeraddr = indexfile.FilePosition();

		// ---- pad the residual name space
		int residual = nodedatalength-classnamesize;
		char *residue = new char[residual];
		memset(residue, 0, residual);
		indexfile.WriteData(residue, residual);
		delete[] residue;

		tmpnode.MarkNodeChanged();
	}
}

//---- register a class's indexes with the database manager
void Parody::RegisterIndexes(Class *cls, const Persistent &pcls)
	// throw (ZeroLengthKey)
{
	Persistent &cl = const_cast<Persistent&>(pcls);
	PdyKey *key = cl.keys.FirstEntry();
	while (key != 0) {
		if (key->GetKeyLength() == 0) {
			// throw ZeroKeyLength()
			// TODO: Error handling
			break;
		}
		PdyBtree *bt = new PdyBtree(indexfile, cls, key);
		bt->SetClassIndexed(cls);
		btrees.AppendEntry(bt);
		key = cl.keys.NextEntry();
	}
}

ClassID Parody::RegisterClass(const Persistent& pcls)
{
	Class *cls = Registration(pcls);
	if (cls == 0) {
		cls = new Class;
		const char *cn = typeid(pcls).name();
		cls->classname = new char[strlen(cn)+1];
		strcpy(cls->classname, cn);

		// ---- search index file for class
		AddClassToIndex(cls);

		// register the indexe
		RegisterIndexes(cls, pcls);

		classes.AppendEntry(cls);
	}

	return cls->classid;
}
	
// =====
// Persistent base class memeber functions
// =====

Persistent *Persistent::objconstructed = 0;
Persistent *Persistent::objdestroyed = 0;
bool Persistent::usingnew = false;


Persistent::Persistent() : parody(*Parody::OpenDatabase())
{
    BuildObject();
}


// ----- common constructor code
void Persistent::BuildObject() throw (NoDatabase)
{
	if (Parody::OpenDatabase() == 0) {
	    printf("ERROR: No database.\n");
	    throw NoDatabase();
	}
	prevconstructed = objconstructed;
	objconstructed = this;
	changed = false;
	deleted = false;
	newobject = false;
	loaded = false;
	saved = false;
	offset = 0;
	indexcount = 0;
	node = 0;
	objectaddress = 0;
	instances = 0;
}

   // ------ destructor
Persistent::~Persistent() throw (NotLoaded, NotSaved, MustDestroy)
{
    if (Parody::OpenDatabase() == 0) {
        // This is causing an error in tests when destroying Parody db
         //throw NoDatabase();
        return;
        // TODO: Error handling
    }
    RemoveObject();
    keys.ClearList();
    delete node;
    if (!loaded) {}
        //throw NotLoaded();
    if (!saved) {}
        //throw NotSaved();
    if (instances != 0) {}
        //throw MustDestroy();
}

void Persistent::Destroy(Persistent *pp)
{
	if (pp != 0) {
          	if (pp->instances == 0) {
            	delete(pp);
            }
            else {
              --(pp->instances);
            }
    }
}

Persistent *Persistent::ObjectBeingConstructed() // throw (NotInConstructor)
{
	Persistent *oc = objconstructed;
    if (oc == 0) {} // throw NotInConstructor()
    return oc;
}

Persistent *Persistent::ObjectBeingDestroyed()
// throw NotInDestructor
{
	Persistent *dc = objdestroyed;
    if (dc == 0) {} //throw NotInDestructor()
    return dc;
}

// ------ search the collected btrees for this key's index
PdyBtree *Persistent::FindIndex(PdyKey *key)
{
  PdyBtree *bt = 0;
  if (key == 0) {
    key = keys.FirstEntry();
  }
  if (key != 0) {
    bt = parody.btrees.FirstEntry();
    while (bt != 0) {
      const char *ty = typeid(*this).name();
      if (strcmp(ty, bt->ClassIndexed()->classname) == 0) {
        if (bt->Indexno() == key->indexno) {
          break;
        }
        bt = parody.btrees.NextEntry();
    }
  } // end while
  } // end if key != 0
  return bt;
}

// ---- remove copies of the original keys
void Persistent::RemoveOrgKeys()
{
    PdyKey *ky = orgkeys.FirstEntry();
    while (ky != 0) {
        delete ky;
        ky = orgkeys.NextEntry();
    }
    orgkeys.ClearList();
}

// ------ record the object's state
void Persistent::RecordObject()
{
    // ---- remove object from the list of instantiated objects
    RemoveOrgKeys();
    // ---- remove copies of the original keys
    parody.objects.RemoveEntry(this);
    // ---- put the object's address in a parody list of
    // instantiated objects
    parody.objects.AppendEntry(this);
    // ---- make copies of the original keys for later update
    PdyKey *key = keys.FirstEntry();
    while (key != 0) {
        PdyKey *ky = key->MakeKey();
        *ky = *key;
        orgkeys.AppendEntry(ky);
        // ---- instantiate the index btree if not already
        FindIndex(ky);
        key = keys.NextEntry();
    }
}

void Persistent::RemoveObject()
{
    // remove object from the list of instantiated objects
    parody.objects.RemoveEntry(this);
    // remove copies of original keys
    RemoveOrgKeys();
}


void Persistent::Read() {
    printf("Persistent Read\n");
}
void Persistent::Write() {
    printf("Persistent Write\n");
}

// ---- remove the record of the object's state
void Persistent::TestDuplicateObject() // throw (Persistent*)
{
    if (objectaddress !=0) {
        // --- search for a previous isntance of this object
        Persistent *obj = parody.objects.FirstEntry();
        while (obj != 0) {
            if (objectaddress == obj->objectaddress) {
                // --- object already instantiated
                obj->instances++;
                saved = true;
                //throw obj;
                return;
            }
            obj = parody.objects.NextEntry();
        }
    }
}

// --- called from derived constructor after all construction
void Persistent::LoadObject(ObjAddr nd)
{
    loaded = true;
    objconstructed = 0;
    objhdr.classid = parody.RegisterClass(*this);
    objectaddress = nd;
    if (parody.rebuildnode) {
        objectaddress = parody.rebuildnode;
    }
    if (objectaddress == 0) {
        // ---- position at object's node
        SearchIndex(keys.FirstEntry());
    }
    ReadDataMembers();
    objconstructed = prevconstructed;
}

// ---- write the object to the database
void Persistent::ObjectOut()
{
    Persistent *hold = objdestroyed;
    objdestroyed = this;
    // ---- tell object to write its data members
    Write();
    objdestroyed = hold;
    // ---- pad the last node
    short int padding = nodedatalength - offset;
    if (padding) {
        char *pads = new char[padding];
        memset(pads, 0, padding);
        parody.datafile.WriteData(pads, padding);
        delete[] pads;
    }
    NodeNbr nx = node->NextNode();
    node->SetNextNode(0);
    delete node;
    node = 0;
    // ---- if node was linked, object got shorter
    while (nx != 0) {
        Node nd(&parody.datafile, nx);
        nx = nd.NextNode();
        nd.MarkNodeDeleted();
    }
    parody.datafile.Seek(filepos);
}

// ---- write the onbject's node header
void Persistent::WriteObjectHeader()
{
    // ---- write the relative node numbera nd class id
    parody.datafile.WriteData(&objhdr, sizeof(ObjectHeader));
    offset = sizeof(ObjectHeader);
}

// ------ read the object's node header
void Persistent::ReadObjectHeader()
{
    // ---- read the relative node number and class id
    parody.datafile.ReadData(&objhdr, sizeof(ObjectHeader));
    offset = sizeof(ObjectHeader);
}

// ---- called from derived destructor before all destruction
// a new or existing object is being saved
void Persistent::SaveObject() throw(NoDatabase)
{
    if (Parody::OpenDatabase() == 0) {
        // throw Nodatabase();
        // TODO: Error
        return;
    }
    saved = true;
    if (parody.rebuildnode) {
        AddIndexes();
        return;
    }
    if (newobject) {
        if (!deleted && ObjectExists()) {
            AddIndexes();
            PositionNode();
            ObjectOut();
            RecordObject(); // TODO: Are these all side effects?!
        }
    }
    // TODO: I think this could be refactored to be more clear.
    // THere are three cases. New, Deleted, and Changed/exists
    else if (deleted || (changed && ObjectExists())) {
        // --- pisition the parody file at the object's node
        PositionNode();
        if (deleted) {
            // delete the object nodes from the datbase
            while (node != 0) {
                node->MarkNodeDeleted();
                NodeNbr nx = node->NextNode();
                delete node;
                if (nx) {
                    node = new Node(&parody.datafile, nx);
                } else {
                    node = 0;
                }
            }
            DeleteIndexes();
            objectaddress = 0;
        }

        else {
            // changed && ObjectExists
            // tell object to write data members
            ObjectOut();
            // --- update the object's indexes
            UpdateIndexes();
            RecordObject();
        }
        parody.datafile.Seek(filepos);
    }
    newobject = false;
    deleted = false;
    changed = false;
}

// ---- read one data member of the object from the database
void Persistent::PdyReadObject(void *buf, short int length)
{
    while (node != 0 && length > 0) {
        if (offset == nodedatalength) {
            NodeNbr nx = node->NextNode();
            delete node;
            node = nx ? new Node(&parody.datafile, nx) : 0;
            ReadObjectHeader();
        }
        if (node != 0) {
            short int len = std::min(length,
                                static_cast<short>(nodedatalength-offset));
            parody.datafile.ReadData(buf, len);
            buf = reinterpret_cast<char*>(buf) + len;
            offset += len;
            length -= len;
        }
    }
}

// ---- write one data member of the object to the database
void Persistent::PdyWriteObject(const void *buf, short length)
{
    while (node != 0 && length > 0) {
        if (offset == nodedatalength) {
            NodeNbr nx = node ->NextNode();
            if (nx == 0) {
                nx = parody.datafile.NewNode();
            }
            node->SetNextNode(nx);
            delete node;
            node = new Node(&parody.datafile, nx);
            WriteObjectHeader();
            objhdr.ndnbr++;
        }
        short int len = std::min(length,
                            static_cast<short>(nodedatalength-offset));
        parody.datafile.WriteData(buf, len);
        buf = reinterpret_cast<const char*>(buf) + len;
        offset += len;
        length -= len;
    }
}

void Persistent::ReadStrObject(std::string &str)
{
    short int len;
    PdyReadObject(&len, sizeof(short int));
    char *s = new char[len+1];
    PdyReadObject(s, len);
    s[len] = '\0';
    str = s;
    delete[] s;
}

void Persistent::WriteStrObject(const std::string &str)
{
    short int len = strlen(str.c_str());
    PdyWriteObject(&len, sizeof(short int));
    PdyWriteObject(str.c_str(), len);
}

// ----- add the index values to the object's index btrees
void Persistent::AddIndexes()
{
    PdyKey *key = keys.FirstEntry();
    while (key != 0) {
        if (!key->isNullValue()) {
            PdyBtree *bt = FindIndex(key);
            key->fileaddr = objectaddress;
            bt->Insert(key);
        }
        key = keys.NextEntry();
    }
}

// ---- update the index values in the object's index btrees
void Persistent::UpdateIndexes()
{
    PdyKey *oky = orgkeys.FirstEntry();
    PdyKey *key = keys.FirstEntry();
    while (key != 0) {
        if (!(*oky == *key)) {
            // ---- key value has changed update the index
            PdyBtree *bt = FindIndex(oky);
            // --- delete the old
            if (!oky->isNullValue()) {
                oky->fileaddr = objectaddress;
                bt->Delete(oky);
            }
            // --- insert the new
            if (!key->isNullValue()) {
                key->fileaddr = objectaddress;
                bt->Insert(key);
            }
        }
        oky = orgkeys.NextEntry();
        key = keys.NextEntry();
    }
}

// --- delete the index values from the objet's index btrees
void Persistent::DeleteIndexes()
{
    PdyKey *oky = orgkeys.FirstEntry();
    while (oky != 0) {
        if (!oky->isNullValue()) {
            PdyBtree *bt = FindIndex(oky);
            oky->fileaddr = objectaddress;
            bt->Delete(oky);
        }
        oky = orgkeys.NextEntry();
    }
}

// ---- position the file to the specified node number
void Persistent::PositionNode()
// throw (BadObjAddr)
{
    filepos = parody.datafile.FilePosition();
    if (objectaddress) {
        delete node;
        node = new Node(&parody.datafile, objectaddress);
        offset = sizeof(ObjectHeader);
        ObjectHeader oh;
        parody.datafile.ReadData(&oh, sizeof(ObjectHeader));
        if (oh.ndnbr != 0 || oh.classid != objhdr.classid) { }
            // throw Bad objaddr TODO: Exception error handling
    }
}

// ----- search the index for a match on the key
void Persistent::SearchIndex(PdyKey *key)
{
    objectaddress = 0;
    if (key != 0 && !key->isNullValue()) {
        PdyBtree *bt = FindIndex(key);
        if (bt != 0 && bt->Find(key)) {
            if (key->indexno != 0) {
                PdyKey *bc;
                do  // TODO: !!!!!
                    bc = bt->Previous();
                while (bc != 0 && *bc == *key);
                key = bt->Next();
            }
            objectaddress = key->fileaddr;
        }
    }
}

// --- scan nodes forward to the first one of the next object
void Persistent::ScanForward(NodeNbr nd)
{
    ObjectHeader oh;
    while (nd++ < parody.datafile.HighestNode()) {
        parody.GetObjectHeader(nd, oh);
        if (oh.classid == objhdr.classid && oh.ndnbr == 0) {
            objectaddress = nd;
            break;
        }
    }
}

// ---- scan nodes back to first one of the previous object
void Persistent::ScanBackward(NodeNbr nd)
{
    ObjectHeader oh;
    while (--nd > 0) {
        parody.GetObjectHeader(nd, oh);
        if (oh.classid == objhdr.classid && oh.ndnbr == 0) {
            objectaddress = nd;
            break;
        }
    }
}

// ------ find an object by a key value
Persistent &Persistent::FindObject(PdyKey *key)
{
    RemoveObject();
    SearchIndex(key);
    ReadDataMembers();
    return *this;
}

// ----- retrieve the current object in a key sequence
Persistent &Persistent::CurrentObject(PdyKey *key)
        {
    RemoveObject();
    PdyBtree *bt = FindIndex(key);
    if (bt != 0) {
        if ((key = bt->Current()) != 0) {
            objectaddress = key->fileaddr;
        }
        ReadDataMembers();
    }
    return *this;
}

// --- retrieve the first object in a key sequence
Persistent &Persistent::FirstObject(PdyKey *key)
{
    RemoveObject();
    objectaddress = 0;
    PdyBtree *bt = FindIndex(key);
    if (bt == 0)  {
        // keyless object
        ScanForward(0);
    }
    else {
        if ((key = bt->First()) != 0) {
            objectaddress = key->fileaddr;
        }
    }
    ReadDataMembers();
    return *this;
}

//---- retrieve the last object in a key sequence
Persistent &Persistent::LastObject(PdyKey *key)
{
    RemoveObject();
    objectaddress = 0;
    PdyBtree *bt = FindIndex(key);
    if (bt == 0)  {
        // --- keyless object
        ScanBackward(parody.datafile.HighestNode());
    }
    else {
        if ((key = bt->Last()) != 0) {
            objectaddress = key->fileaddr;
        }
    }
    ReadDataMembers();
    return *this;
}

// --- retrieve the next object in the key sequence
Persistent &Persistent::NextObject(PdyKey *key)
{
    RemoveObject();
    ObjAddr oa = objectaddress;
    PdyBtree *bt = FindIndex(key);
    if (bt == 0) {
        // ---- keyless object
        ScanForward(oa);
    }
    else {
        if ((key = bt->Next()) != 0) {
            objectaddress = key->fileaddr;
        }
    }
    ReadDataMembers();
    return *this;
}

// --- -retreive the previous object in the key sequence
Persistent &Persistent::PreviousObject(PdyKey *key)
{
    RemoveObject();
    ObjAddr oa = objectaddress;
    objectaddress = 0;
    PdyBtree *bt = FindIndex(key);
    if (bt == 0) {
        // ---- keyless object
        ScanBackward(oa);
    } else {
        if ((key = bt->Previous()) != 0) {
            objectaddress = key->fileaddr;
        }
    }
    ReadDataMembers();
    return *this;
}

// ------ read an object's data members
void Persistent::ReadDataMembers()
{
    if (objectaddress != 0) {
        PositionNode();
        // ---- tell object to read its data members
        Persistent *hold = objconstructed;
        objconstructed = this;
        Read();
        objconstructed = hold;
        delete node;
        node = 0;
        TestDuplicateObject();
        // ---- post object instantiated
        // and put secondary keys in table
        RecordObject();
        parody.datafile.Seek(filepos);
    }
}

// ------ add an object to the Parody Database
bool Persistent::AddObject()
{
    newobject = (objectaddress == 0 && TestRelationships());
    if (newobject) {
        delete node; // (just in case)
        node = new Node(&parody.datafile, parody.datafile.NewNode());
        objectaddress = node->GetNodeNbr();
        WriteObjectHeader();
        objhdr.ndnbr++;
    }
    return newobject;
}

// ------ mark a persistent object for a change
bool Persistent::ChangeObject()
{
    changed = TestRelationships();
    return changed;
}

// ----- mark a persistent object for a delete
bool Persistent::DeleteObject()
{
    PdyKey *key = keys.FirstEntry();
    bool related = false;

    if (!key->isNullValue()) {
        // --- scan for other objects related to this one
        PdyBtree *bt = parody.btrees.FirstEntry();
        while (bt != 0 && !related) {
            // test only secondary keys
            if (bt->Indexno() != 0) {
                const std::type_info *relclass = bt->NullKey() ->relatedclass;

                if (relclass != 0) {
                    if (typeid(*this) == *relclass) {
                        PdyKey *ky = bt->MakeKeyBuffer();
                        if (ky->isObjectAddress()) {
                            const ObjAddr *oa = ky->ObjectAddress();
                            ObjectHeader oh;
                            parody.GetObjectHeader(*oa, oh);
                            if (oh.classid == objhdr.classid) {
                                if (oh.ndnbr == 0) { related = true; }
                            }
                        }
                        else {
                            ky->CopyKeyData(*key);
                            related = bt->Find(ky);
                        }
                    }
                }
            }
            bt = parody.btrees.NextEntry();
        }
    }
    deleted = !related;
    return deleted;
}

// ------ test an object's relationships
//        return false if it is related to a
//        nonexistent object
//      return false if its primary key is already in use
bool Persistent::TestRelationships()
{
    PdyKey *key = keys.FirstEntry();
    if (key == 0)  {
        return true;
    }
    PdyBtree *bt;
    if (objectaddress == 0) {
        bt = FindIndex(key);
        if (bt !=0 && bt->Find(key)) {
            return false;
        }
    }
    bool unrelated = true;

    while ((key = keys.NextEntry()) != 0) {
        const std::type_info *relclass = key->relatedclass;
        if (key->isObjectAddress()) {
            const ObjAddr *oa = key->ObjectAddress();
            if (oa != 0) {
                ObjectHeader oh;
                parody.GetObjectHeader(*oa, oh);
                if (oh.ndnbr == 0) {
                    // --- find classid of related class
                    Class *cls = parody.classes.FirstEntry();
                    const char *cn = relclass->name();
                    while (cls != 0) {
                        if (strcmp(cn, cls->classname)==0) {
                            break;
                        }
                        cls = parody.classes.NextEntry();
                    }
                    if (cls && cls->classid == oh.classid) {
                        continue;
                    }
                }
                unrelated = false;
            }
        }
        else
        {
            if (!key->isNullValue() && relclass != 0) {
                const char *kc = relclass->name();
                bt = parody.btrees.FirstEntry();
                while (bt != 0 && unrelated) {
                    // ---- test only primary keys
                    if (bt->Indexno() == 0) {
                        // -- primary key of related class?
                        const char *bc = bt->ClassIndexed()->classname;
                        if (strcmp(bc, kc) == 0) {
                            PdyKey *ky = bt->MakeKeyBuffer();
                            ky->CopyKeyData(*key);
                            unrelated = bt->Find(ky);
                        }
                    }
                    bt = parody.btrees.NextEntry();
                }

            }
        }
    }
    return unrelated;
}
