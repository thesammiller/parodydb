// ------ key.h

//#include "typeid.h"

#ifndef KEY_H
#define KEY_H

#include "node.h"
#include "structs.h"

// ======
// PdyKey abstract base class
// ======
class PdyKey {
public:
	NodeNbr fileaddr; // object address -> by this key
	NodeNbr lowernode; // lower node of keys > this key
	virtual void WriteKey(IndexFile &bfile) = 0;
    virtual void ReadKey(IndexFile &bfile) = 0;
    // isNullValue was virtual and causing problem for inheritance
    bool isNullValue() { return false; };
    // CopyKeyData also used to be virtual, but that caused problems
    void CopyKeyData(const PdyKey &key) {};
    virtual bool isObjectAddress() const = 0;
    virtual const ObjAddr *ObjectAddress() const = 0;
    virtual PdyKey *MakeKey() const = 0;
//protected:
    const std::type_info *relatedclass; // related class
    IndexNo indexno; // 0=primary key, >0 = secondary key
    KeyLength keylength; // length of key
//public:
    PdyKey(NodeNbr fa = 0);
    virtual ~PdyKey()
        {/*...*/}
    virtual int operator>(const PdyKey &key) const = 0;
    virtual int operator==(const PdyKey &key) const = 0;
    virtual PdyKey &operator=(const PdyKey& key);
    // TODO: I don't think this is used in the codebase, double check
    void Relate(const std::type_info *ti)
        { relatedclass = ti; }
    KeyLength GetKeyLength() const
        { return keylength; }
    void SetKeyLength(KeyLength kylen)
        { keylength = kylen; }
};




// ======
// Key class
// =====
template <class T>
class Key : public PdyKey {
public:
    T ky;
    bool isObjectAddress() const
        { return typeid(T) == typeid(ObjAddr); }
    const ObjAddr *ObjectAddress() const
        { return reinterpret_cast<const ObjAddr*>(&ky); }
    virtual void CopyKeyData(const PdyKey &key);
    Key<T> *MakeKey() const;
//public:
    Key(const T& key);
    virtual ~Key()
        { /* ... */ }
    PdyKey &operator=(const PdyKey &key);
    int operator>(const PdyKey &key) const;
    int operator==(const PdyKey& key) const;
    T& KeyValue()
        { return ky; }
    void SetKeyValue(const T& key)
        { ky = key; }
    const T& KeyValue() const
        { return ky; }
    virtual void WriteKey(IndexFile &ndx);
    virtual void ReadKey(IndexFile &ndx);
    bool isNullValue() const;
};



template <class T>
Key<T>::Key(const T& key) : ky(key)
{
    keylength = sizeof(T);
}



template <class T>
void Key<T>::CopyKeyData(const PdyKey &key)
{
    const Key<T> *kp = dynamic_cast<const Key<T>*>(&key);
    ky = kp->ky;
}

template <class T>
PdyKey &Key<T>::operator=(const PdyKey &key)
{
    if (this != &key) {
        PdyKey::operator=(key);
        CopyKeyData(key);
    }
    return *this;
}

template <class T>
int Key<T>::operator>(const PdyKey &key) const
{
    const auto* kp = dynamic_cast<const Key<T>*>(&key);
    if (kp) {
        return ky > kp->ky;
    }
    return false;
}

template <class T>
int Key<T>::operator==(const PdyKey &key) const
{
    const auto* kp = dynamic_cast<const Key<T>*>(&key);
    if (kp) {
        return ky == kp->ky;
    }
    return false;
}

// TODO: Shouldn't this incrememnt a node number or something?
// How does it save to different memory?
template <class T>
Key<T> *Key<T>::MakeKey() const
{
    Key *newkey = new Key<T>(T(0));
    newkey->SetKeyLength(keylength);
    return newkey;
}

// --- ReadKey must be specialized if key != simple data type
template <class T>
void Key<T>::ReadKey(IndexFile &ndx)
{
    if (keylength > 0) {
        ndx.ReadData(&ky, keylength);
    }
}

// --- WriteKey must be specialized if key != simple data type
template <class T>
void Key<T>::WriteKey(IndexFile &ndx)
{
    if (keylength > 0) {
        ndx.WriteData(&ky, keylength);
    }
}

template <class T>
bool Key<T>::isNullValue() const
{
    return ky == T(0);
}

// ======
// specialized Key<string> template member functions
// ======
template<>
inline Key<std::string>::Key(const std::string& key) : ky(key)
{
    keylength = key.length();
}

template<>
inline void Key<std::string>::CopyKeyData(const PdyKey &key)
{
    const auto* kp = dynamic_cast<const Key<std::string>*>(&key);
    if (kp) {
        ky = kp->ky;
        // TODO: Confirm Bugfix
        keylength = ky.length();
    }
}

template<>
inline void Key<std::string>::ReadKey(IndexFile &ndx)
{
    char *cp = new char[keylength+1];
    ndx.ReadData(cp, keylength);
    *(cp+keylength) = '\0';
    ky = std::string(cp);
    delete[] cp;
}

template<>
inline void Key<std::string>::WriteKey(IndexFile &ndx)
{
    ky.resize(keylength);
    ndx.WriteData(ky.c_str(), keylength);
}

template<>
inline Key<std::string> *Key<std::string>::MakeKey() const
{
    std::string newkeyname = std::string(keylength, '\0');
    Key *newkey = new Key<std::string>(newkeyname);
    newkey->SetKeyLength(keylength);
    return newkey;
}

// TODO: Research this, because strings can't be nulls?
template<>
inline bool Key<std::string>::isNullValue() const
{
    return ky == "";
}



// ======
// concatenated key class
// =====

template <class T1, class T2>
class CatKey : public PdyKey {
public:
    Key<T1> ky1;
    Key<T2> ky2;
    bool isObjectAddress() const {
        return false;
    }
    const ObjAddr *ObjectAddress() const {
        return 0;
    }
    void CopyKeyData(const PdyKey &key);
    // ---- readkey/writekey must be specialized
    //    if key(s) != simple data type
    virtual void ReadKey(IndexFile &ndx) {
        ky1.ReadKey(ndx); ky2.ReadKey(ndx);
    }
    virtual void WriteKey(IndexFile &ndx) {
        ky1.WriteKey(ndx); ky2.WriteKey(ndx);
    }
    PdyKey *MakeKey() const;
    bool isNullValue() const
        { return ky1.isNullValue() && ky2.isNullValue(); }
//public:
    CatKey(const T1& key1, const T2& key2);
    ~CatKey() {}
    PdyKey &operator=(const PdyKey &key);
    int operator>(const PdyKey& key) const;
    int operator==(const PdyKey& key) const;
    Key<T1> &Key1()
        { return ky1; }
    T1 &Keyvalue()
        {return ky1.Keyvalue();}
    const T1 &keyvalue() const
        { return ky1.Keyvalue();}
    Key<T2> &Key2()
        { return ky2; }
    T2& Keyvalue2()
        { return ky2.KeyValue(); }
    const T2& KeyValue2() const
        { return ky2.KeyValue(); }
    void SetKeyvalue2(const T2& key2)
        { ky2.SetKeyvalue(key2); }
};

template <class T1, class T2>
CatKey<T1, T2>::CatKey(const T1& key1, const T2& key2)
    : ky1(key1), ky2(key2)
{
    keylength = ky1.GetKeyLength() + ky2.GetKeyLength();
}

// If my.key1 > your.key1 || my.key1 == your.key1 && my.key2 > your.key2
// Basically think of it as big endian with two values
template <class T1, class T2>
int CatKey<T1, T2>::operator>(const PdyKey &key) const
{
    const CatKey<T1, T2> *ckp = static_cast<const CatKey<T1,T2>*>(&key);
    if (ky1 > ckp->ky1) {
        return 1;
    }
    if (ky1 == ckp->ky1 && ky2 > ckp->ky2) {
        return 1;
    }
    return 0;
}

template <class T1, class T2>
int CatKey<T1,T2>::operator==(const PdyKey &key) const
{
    const CatKey<T1, T2> *ckp = static_cast<const CatKey<T1, T2>*>(&key);
    return ky1 == ckp->ky1 && ky2 == ckp->ky2;
}

template <class T1, class T2>
void CatKey<T1, T2>::CopyKeyData(const PdyKey &key)
{
    const CatKey<T1, T2> *ckp = dynamic_cast<const CatKey<T1, T2>*>(&key);
    ky1 = ckp->ky1;
    ky2 = ckp->ky2;
}

template <class T1, class T2>
PdyKey& CatKey<T1, T2>::operator=(const PdyKey &key)
{
    if (this != &key) {
        PdyKey::operator=(key);
        CopyKeyData(key);
    }
    return *this;
}

template <class T1, class T2>
PdyKey *CatKey<T1, T2>::MakeKey() const
{
    CatKey<T1,T2> *newkey = new CatKey<T1, T2>(T1(0), T2(0));
    newkey->ky1.SetKeyLength(ky1.GetKeyLength());
    newkey->ky2.SetKeyLength(ky2.GetKeyLength());
    newkey->SetKeyLength(keylength);
    return newkey;
}


#endif
