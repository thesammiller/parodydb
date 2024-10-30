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


template <class T>
Key<T>::Key(const T& key) : ky(key)
{
    keylength = sizeof(T);
}

template <class T>
void Key<T>::CopyKeyData(const PdyKey &key)
{
    const Key<T> *kp = static_cast<const Key<T>*>(key);
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
    const auto* kp = dynamic_cast<const Key<std::string>*>(&key);
    if (kp) {
        return ky > kp->ky;
    }
    return false;
}

template <class T>
int Key<T>::operator==(const PdyKey &key) const
{
    const auto* kp = dynamic_cast<const Key<std::string>*>(&key);
    if (kp) {
        return ky == kp->ky;
    }
    return false;
}

template <class T>
PdyKey *Key<T>::MakeKey() const
{
    PdyKey *newkey = new Key<T>(T(0));
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
//inline
template<>
Key<std::string>::Key(const std::string& key) : ky(key)
{
    keylength = key.length();
}

template<>
void Key<std::string>::CopyKeyData(const PdyKey &key)
{
    const auto* kp = dynamic_cast<const Key<std::string>*>(&key);
    if (kp) {
        ky = kp->ky;
    }
}

//inline
template<>
void Key<std::string>::ReadKey(IndexFile &ndx)
{
    char *cp = new char[keylength+1];
    ndx.ReadData(cp, keylength);
    *(cp+keylength) = '\0';
    ky = std::string(cp);
    delete[] cp;
}

//inline
template<>
void Key<std::string>::WriteKey(IndexFile &ndx)
{
    ky.resize(keylength);
    ndx.WriteData(ky.c_str(), keylength);
}

//inline
template<>
PdyKey *Key<std::string>::MakeKey() const
{
    PdyKey *newkey = new Key<std::string>(std::string('\0', keylength));
    newkey->SetKeyLength(keylength);
    return newkey;
}

//inline
// TODO: Research this, because strings can't be nulls?
template<>
bool Key<std::string>::isNullValue() const
{
    return ky == "";
}


