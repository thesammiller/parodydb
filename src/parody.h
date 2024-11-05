// ----- parody.h

#ifndef DATABASE_H
#define DATABASE_H

#include <fstream>
#include <typeinfo>
#include <cstring>


// ===
// Parody exceptions representing program errors
// ===

class PdyExceptions {};
// read object
class NotInConstructor : public PdyExceptions {};
// write object
class NotInDestructor : public PdyExceptions {};
// no db open
class NoDatabase : public PdyExceptions {};
// load object not called
class NotLoaded : public PdyExceptions {};
// save object not called
class NotSaved : public PdyExceptions {};
// multi-reference object deleted
class MustDestroy : public PdyExceptions {};
// string key w/out size
class ZeroLengthKey : public PdyExceptions {};
// key length != btree key length
class BadKeyLength : public PdyExceptions {};
// bad reference object
class badReference : public PdyExceptions {};
// bad object address specified
class BadObjAddr : public PdyExceptions {};

// ====
// class id
// ====
//typedef short int ClassID;

// ==
// class id struct
// ===


// ====
// key controls
// ====

//#include "bool.h"
#include "linklist.h"
#include "btree.h"

class Parody;

//====
// persistent object abstract base class
// ====
class Persistent {
	friend class Parody;
	friend class PdyKey;
	friend class PdyReference;
public:
	ObjectHeader objhdr;
	ObjAddr objectaddress; // Node address for this object
	Parody& parody; // database for this object
	short int indexcount; // number of keys in the object
	short int instances; // number of instances of object
	Node *node; // current node for reading writing
	short int offset; // current char position
	bool changed; // true if user changed the object
	bool deleted; // true if user delted the object
	bool newobject; // true if user is adding the object
	bool loaded;
	bool saved;
	static bool usingnew;
	std::streampos filepos;

	// --- pointers to associate keys with objects
	Persistent *prevconstructed;
	static Persistent *objconstructed;
	static Persistent *objdestroyed;

	LinkedList<PdyKey> keys;
	LinkedList<PdyKey> orgkeys; // original keys in the object

	// ---- private copy constructor
	//Persistent(Persistent&) : parody(Parody())
	//	{ /* ... */ }
	Persistent& operator=(Persistent&)
		{ return *this; }
    Persistent(Persistent&) = delete;


	// --- methods used from within Persistent class
	void RegisterKey(PdyKey *key)
		{ keys.AppendEntry(key); }
	void ObjectOut();
	void RecordObject();
	void RemoveObject();
    void RemoveOrgKeys();
	void AddIndexes();
	void DeleteIndexes();
	void UpdateIndexes();
	void PositionNode(); //throw (BadObjAddr);
	void ReadObjectHeader();
	void WriteObjectHeader();
	void SearchIndex(PdyKey *key);
	void ReadDataMembers();
	PdyBtree *FindIndex(PdyKey *key);
	bool TestRelationships();
	void ScanForward(NodeNbr nd);
	void ScanBackward(NodeNbr nd);
	void BuildObject(); //throw (NoDatabase);
	void TestDuplicateObject(); // throw (Persistent*);
//public:
	// --- these are public members because template functions
	// cannot be friends or member functions	
	static Persistent *ObjectBeingConstructed();
		// throw (NotInConstructor);
	static Persistent *ObjectBeingDestroyed();
		// throw (NotInDestructor);
	void PdyReadObject(void *buf, short int length);
	void PdyWriteObject(const void *buf, short int length);
	void ReadStrObject(std::string &str);
	void WriteStrObject(const std::string &str);
	// ---- s/b called only from reference template
	void AddReference()
		{ instances++; }
//protected:
	Persistent();
	Persistent(Parody &db);
	virtual ~Persistent();
		// throw (NotLoaded, NotSaved, MustDestroy);
	// ---- provided by derived class
	virtual void Write() = 0;
	virtual void Read() = 0;
	// called from derived class's constructor
	void LoadObject(ObjAddr nd = 0);
//public
	// --- called from derived class's destructor
	// or by user to force output to db
	void SaveObject(); // throw (NoDatabase);
	
	// ---- class interface methods for modifying database
	bool AddObject();
	bool ChangeObject();
	bool DeleteObject();
	bool ObjectExists() const
		{ return objectaddress != 0; }

	// ---- class interface methods for searching db
	Persistent& FindObject(PdyKey *key);
	Persistent& CurrentObject(PdyKey *key = 0);
	Persistent& FirstObject(PdyKey *key = 0);
	Persistent& LastObject(PdyKey *key = 0);
	Persistent& NextObject(PdyKey *key = 0);
	Persistent& PreviousObject(PdyKey *key = 0);
	// ---- return the object indentification
	ObjAddr ObjectAddress() const 
		{ return objectaddress; }
	// ---- pseudo delete operator for multiple instances
	static void Destroy(Persistent *pp);
};


// ======= 
// DataFile class
// ======

class DataFile : public NodeFile {
public:
	DataFile(const std::string& name) : NodeFile(name+".dat")
		{/* .... */}
};

// ======
// Parody Database
// ======

class Parody {
public:
	friend Persistent;
	DataFile datafile;	// the object datafile
	IndexFile indexfile; 	// the b-tree file
	LinkedList<Persistent> objects; // instantiated objects
	LinkedList<Class> classes; 	// registered classes
	LinkedList<PdyBtree> btrees;	// btrees in the database
	// ---- for Index program to rebuild indexes
	ObjAddr rebuildnode;	// object being rebuilt
	Parody *previousdatabase; 	// previous open database
	static Parody *opendatabase;	// latest open database
	void GetObjectHeader(ObjAddr nd, ObjectHeader &objhdr);
	void RebuildIndexes(ObjAddr nd)
		{ rebuildnode = nd; }
	bool FindClass(Class *cls, NodeNbr *nd= 0);
	ClassID GetClassID(const char *classname);
	//friend void BuildIndex();
	// ---- private copy constructor & assignment prevent copies
	Parody(Parody &) : datafile(std::string()), indexfile(std::string())
		{ /* ... */ }
	Parody &operator=(Parody &)
		{ return *this; }
	void RegisterIndexes(Class *cls, const Persistent& pcls);
		// throw (ZeroLengthKey);
	ClassID RegisterClass(const Persistent& cls);
	Class *Registration(const Persistent& pcls);
	void AddClassToIndex(Class *cls);
//public:
	Parody(const std::string &name);
	~Parody();
	static Parody *OpenDatabase()
		{ return opendatabase; }
};

// Persistent constructor using last declared database
//inline 

// ---- persistent constructor using specified database
//inline 
//Persistent::Persistent(Parody &db) : parody(db)
//{
//	BuildObject();
//}



// ======
// PersistentObject template
// =====

template <class T>
class PersistentObject : public Persistent {
	void Read()
		{PdyReadObject(reinterpret_cast<void*>(&Obj), sizeof(T));}
	void Write()
		{PdyWriteObject(reinterpret_cast<void*>(&Obj), sizeof(T));}
public:
	T Obj;
	PersistentObject(const T& obj) : Obj(obj)
		{LoadObject();}
	PersistentObject(ObjAddr oa = 0)
		{LoadObject(oa);}
	virtual ~PersistentObject()
		{SaveObject();}
};

// ======
// Reference template
// ======
template <class T>
class Reference {
public:
	T *obj;
	Reference();
    ~Reference();
	void ReadObject();
	void WriteObject();
	void operator=(T& to); // throw (BadReference);
	void RemoveReference();
};

template <class T>
Reference<T>::Reference()
{
	obj = 0;
}

template <class T>
Reference<T>::~Reference()
{
	Persistent::Destroy(obj);
}


template <class T>
void ReadObject(T &t)
{
    Persistent *oc = Persistent::ObjectBeingConstructed();
    oc->PdyReadObject(&t, sizeof(T));
}

template <class T>
void WriteObject(const T& t)
{
    Persistent *od = Persistent::ObjectBeingDestroyed();
    od->PdyWriteObject(&t, sizeof(T));
}

//inline
void ReadObject(std::string &s)
{
    Persistent *oc = Persistent::ObjectBeingConstructed();
    oc->ReadStrObject(s);
}

inline void WriteObject(const std::string &s)
{
    Persistent *od = Persistent::ObjectBeingDestroyed();
    od->WriteStrObject(reinterpret_cast<const std::string&>(s));
}

template <class T>
void Reference<T>::ReadObject()
{
	Persistent::Destroy(obj);
	obj = 0;

	ObjAddr oa;
	::ReadObject(oa);
	if (oa != 0) { 
		obj = new T(oa); 
	}
}

template <class T>
void Reference<T>::WriteObject()
{
	ObjAddr oa = 0;
	if (obj != 0) {
		oa = obj->ObjectAddress();
	}
	::WriteObject(oa);
}

template <class T>
void Reference<T>::operator=(T &to)
	//throw (BadReference)
{
	Persistent *po = dynamic_cast<Persistent*>(&to);
	if (po == 0) {
		//throw BadReference();
		// TODO: log error, fix exceptions
	}
	Persistent::Destroy(obj);
	obj = static_cast<T*>(po);
	obj->AddReference();
}

template <class T>
void Reference<T>::RemoveReference()
{
	Persistent::Destroy(obj);
	obj = 0;
}


#endif
