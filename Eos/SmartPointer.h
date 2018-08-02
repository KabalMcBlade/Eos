#pragma once

#include "CoreDefs.h"
#include "MemoryManager.h"


EOS_NAMESPACE_BEGIN

template struct EOS_DLL std::atomic<uint_fast32_t>;

class EOS_DLL SmartObject
{
public:
    typedef eosU32 RefCount;

    static const RefCount K_INVALID_REFCOUNT = (RefCount)-1;

    SmartObject() : m_referenceCount(0) {}
    virtual ~SmartObject() {}

    EOS_INLINE RefCount RefIncrement()
    {
        return m_referenceCount.fetch_add(1, std::memory_order_relaxed);
    }

    EOS_INLINE RefCount RefDecrement()
    {
        return m_referenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

    EOS_INLINE RefCount GetRefCount() const
    {
        return m_referenceCount.load();
    }

private:
    typedef std::atomic_uint_fast32_t   Atomic;

    mutable Atomic m_referenceCount;
};


//////////////////////////////////////////////////////////////////////////


template <typename T>
class SmartPointer
{
public:
    SmartPointer() : m_pObject(nullptr) {}

    SmartPointer(T* _pObject) : m_pObject(_pObject)
    {
        RefIncrement(m_pObject);
    }

    template <typename U>
    SmartPointer(SmartPointer<U> & _x) : m_pObject(_x.GetPtr())
    {
        RefIncrement(m_pObject);
    }

    SmartPointer(SmartPointer const & _Source) : m_pObject(_Source.m_pObject)
    { 
        RefIncrement(m_pObject);
    }

    SmartPointer(SmartPointer && _Source) : m_pObject(_Source.m_pObject)
    { 
        _Source.m_pObject = nullptr; 
    }

    ~SmartPointer()
    {
        Release();
    }

    SmartPointer & operator=(SmartPointer const &_Source)
    { 
        SmartPointer(_Source).Swap(*this);
        return *this; 
    }

    SmartPointer & operator=(SmartPointer && _Source)
    { 
        SmartPointer(std::forward<SmartPointer>(_Source)).Swap(*this);
        return *this; 
    }

    SmartPointer & operator=(T* _pSource)
    { 
        SmartPointer(_pSource).Swap(*this);
        return *this; 
    }

    void Release()
    {
        if (m_pObject == nullptr)
        {
            return;
        }

        // auto destroy object allocated
        const SmartObject::RefCount refCount = RefDecrement(m_pObject);

        if (refCount == 0)
        {
            eosDelete(m_pObject);
        }

        m_pObject = nullptr;
    }

    eosBool IsValid() const { return m_pObject != nullptr; }
    eosU32 GetRefCount() const { return IsValid() ? GetRefCount(m_pObject) : 0; }

    T& operator*() const { return *m_pObject; }
    T* operator->() const { return  m_pObject; }

    // accessor for general purpose
    // be careful!
    T& Get() const { return *m_pObject; }
    T* GetPtr() const { return  m_pObject; }

protected:
    EOS_INLINE SmartObject::RefCount RefIncrement(SmartObject* _pObject)
    {
        return (_pObject) ? _pObject->RefIncrement() : SmartObject::K_INVALID_REFCOUNT;
    }

    EOS_INLINE SmartObject::RefCount RefDecrement(SmartObject* _pObject)
    {
        return (_pObject) ? _pObject->RefDecrement() : SmartObject::K_INVALID_REFCOUNT;
    }

    EOS_INLINE SmartObject::RefCount GetRefCount(SmartObject* _pObject)
    {
        return (_pObject) ? _pObject->GetRefCount() : SmartObject::K_INVALID_REFCOUNT;
    }

    void Swap(SmartPointer &_source)
    {
        std::swap(m_pObject, _source.m_pObject);
    }

    T * m_pObject;
};

template <class T1, class T2> EOS_INLINE bool operator==(SmartPointer<T1> const & _sp1, SmartPointer<T2> const & _sp2) { return _sp1.GetPtr() == _sp2.GetPtr(); }
template <class T1, class T2> EOS_INLINE bool operator==(SmartPointer<T1> const & _sp1, T2* _p2) { return _sp1.GetPtr() == _p2; }
template <class T1, class T2> EOS_INLINE bool operator==(T1* _p1, SmartPointer<T2> const & _sp2) { return _p1 == _sp2.GetPtr(); }

template <class T1, class T2> EOS_INLINE bool operator!=(SmartPointer<T1> const & _sp1, SmartPointer<T2> const & _sp2) { return _sp1() != _sp2(); }
template <class T1, class T2> EOS_INLINE bool operator!=(SmartPointer<T1> const & _sp1, T2* _p2) { return _sp1.GetPtr() != _p2; }
template <class T1, class T2> EOS_INLINE bool operator!=(T1* _p1, SmartPointer<T2> const & _sp2) { return _p1 != _sp2.GetPtr(); }

template <class T>EOS_INLINE bool operator<(SmartPointer<T> const & _sp1, SmartPointer<T> const & _sp2) { return _sp1.GetPtr() < _sp2.GetPtr(); }
template <class T>EOS_INLINE bool operator>(SmartPointer<T> const & _sp1, SmartPointer<T> const & _sp2) { return _sp1.GetPtr() > _sp2.GetPtr(); }

template <class T, eosSize Alignment, typename... Args>
class AutoSmartPointer : public SmartPointer<T>
{
public:
    AutoSmartPointer(Args... args) : SmartPointer()
    {
        m_pObject = eosNew(T, Alignment, args...);
        RefIncrement(m_pObject);
    }

    template <class rhsT, eosSize Alignment>
    AutoSmartPointer(AutoSmartPointer<rhsT, Alignment> & _rhs) : SmartPointer(_rhs())
    {
        RefIncrement(m_pObject);
    }
};


template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator==(AutoSmartPointer<T1, Alignment1> const & _sp1, AutoSmartPointer<T2, Alignment2> const & _sp2) { return _sp1() == _sp2(); }
template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator==(AutoSmartPointer<T1, Alignment1> const & _sp1, T2* _p2) { return _sp1() == _p2; }
template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator==(T1* _p1, AutoSmartPointer<T2, Alignment2> const & _sp2) { return _p1 == _sp2(); }

template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator!=(AutoSmartPointer<T1, Alignment1> const & _sp1, AutoSmartPointer<T2, Alignment2> const & _sp2) { return _sp1() != _sp2(); }
template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator!=(AutoSmartPointer<T1, Alignment1> const & _sp1, T2* _p2) { return _sp1() != _p2; }
template <class T1, eosSize Alignment1, class T2, eosSize Alignment2> EOS_INLINE bool operator!=(T1* _p1, AutoSmartPointer<T2, Alignment2> const & _sp2) { return _p1 != _sp2(); }

template <class T, eosSize Alignment>EOS_INLINE bool operator<(AutoSmartPointer<T, Alignment> const & _sp1, AutoSmartPointer<T, Alignment> const & _sp2) { return std::less<T*>(_sp1(), _sp2()); }
template <class T, eosSize Alignment>EOS_INLINE bool operator>(AutoSmartPointer<T, Alignment> const & _sp1, AutoSmartPointer<T, Alignment> const & _sp2) { return std::greater<T*>(_sp1(), _sp2()); }

EOS_NAMESPACE_END