#pragma once

#include "CoreDefs.h"

#include "MemoryFunctions.h"

EOS_NAMESPACE_BEGIN


template struct EOS_DLL std::atomic<uint32_t>;

class EOS_DLL eosSmartObject
{
public:
    typedef eosU32 RefCount;

    static const RefCount kInvalidRefCount = (RefCount)-1;

    eosSmartObject() : m_referenceCount(0) {}
    virtual ~eosSmartObject() {}

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
    mutable std::atomic_uint32_t m_referenceCount;
};


//////////////////////////////////////////////////////////////////////////


template <typename T, typename Allocator>
class eosSmartPointer
{
private:
    template<typename U, typename Allocator>
    friend class eosSmartPointer;

public:
    eosSmartPointer(Allocator* _allocator, T* _value) : m_allocator(_allocator), m_object(_value)
    {
        // do not need to track who allocated, I keep the allocator anyway
        RefIncrement(m_object);
    }

    eosSmartPointer(Allocator* _allocator) : m_allocator(_allocator)
    {
        m_object = eosNew(T, m_allocator);
        RefIncrement(m_object);
    }

    template <typename U>
    eosSmartPointer(eosSmartPointer<U, Allocator> const & _other) : m_allocator(_other.m_allocator), m_object(_other.m_object)
    {
        RefIncrement(m_object);
    }

    eosSmartPointer(eosSmartPointer const & _other) : m_allocator(_other.m_allocator), m_object(_other.m_object)
    { 
        RefIncrement(m_object);
    }

//     eosSmartPointer(eosSmartPointer && _other) : m_allocator(std::move(_other.m_allocator)), m_object(std::move(_other.m_object))
//     { 
//         _other.m_allocator = nullptr;
//         _other.m_object = nullptr;
//     }

    ~eosSmartPointer()
    {
        Release();
    }

    eosSmartPointer & operator=(eosSmartPointer const &_other)
    { 
        eosSmartPointer(_other).Swap(*this);
        return *this; 
    }

//     eosSmartPointer & operator=(eosSmartPointer && _other)
//     { 
//         eosSmartPointer(std::forward<eosSmartPointer>(_other)).Swap(*this);
//         return *this; 
//     }

    eosSmartPointer & operator=(T* _other)
    { 
        eosSmartPointer(_other).Swap(*this);
        return *this; 
    }

    void Release()
    {
        eosAssertDialog(m_allocator);

        if (m_object == nullptr)
        {
            return;
        }

        // auto destroy object allocated
        const eosSmartObject::RefCount refCount = RefDecrement(m_object);

        if (refCount == 0)
        {
            eosDelete(m_object, m_allocator);
        }

        m_object = nullptr;
    }

    eosBool IsValid() const { return m_object != nullptr; }
    eosU32 GetRefCount() const { return IsValid() ? GetRefCount(m_object) : 0; }

    T& operator*() const { return *m_object; }
    T* operator->() const { return  m_object; }

    // accessor for general purpose
    // be careful!
    T& Get() const { return *m_object; }
    T* GetPtr() const { return  m_object; }

protected:
    EOS_INLINE eosSmartObject::RefCount RefIncrement(eosSmartObject* _object)
    {
        return (_object) ? _object->RefIncrement() : eosSmartObject::kInvalidRefCount;
    }

    EOS_INLINE eosSmartObject::RefCount RefDecrement(eosSmartObject* _object)
    {
        return (_object) ? _object->RefDecrement() : eosSmartObject::kInvalidRefCount;
    }

    EOS_INLINE eosSmartObject::RefCount GetRefCount(eosSmartObject* _object)
    {
        return (_object) ? _object->GetRefCount() : eosSmartObject::kInvalidRefCount;
    }

    void Swap(eosSmartPointer &_source)
    {
        std::swap(m_allocator, _source.m_allocator);
        std::swap(m_object, _source.m_object);
    }

    Allocator* m_allocator;
    T * m_object;
};

template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(eosSmartPointer<T1, typename Allocator> const & _sp1, eosSmartPointer<T2, typename Allocator> const & _sp2) { return _sp1->GetPtr() == _sp2->GetPtr(); }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(eosSmartPointer<T1, typename Allocator> const & _sp1, T2* _p2) { return _sp1->GetPtr() == _p2; }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(T1* _p1, eosSmartPointer<T2, typename Allocator> const & _sp2) { return _p1 == _sp2->GetPtr(); }

template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(eosSmartPointer<T1, typename Allocator> const & _sp1, eosSmartPointer<T2, typename Allocator> const & _sp2) { return _sp1() != _sp2(); }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(eosSmartPointer<T1, typename Allocator> const & _sp1, T2* _p2) { return _sp1->GetPtr() != _p2; }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(T1* _p1, eosSmartPointer<T2, typename Allocator> const & _sp2) { return _p1 != _sp2->GetPtr(); }

template <class T, typename Allocator>EOS_INLINE bool operator<(eosSmartPointer<T, typename Allocator> const & _sp1, eosSmartPointer<T, typename Allocator> const & _sp2) { return _sp1->GetPtr() < _sp2->GetPtr(); }
template <class T, typename Allocator>EOS_INLINE bool operator>(eosSmartPointer<T, typename Allocator> const & _sp1, eosSmartPointer<T, typename Allocator> const & _sp2) { return _sp1->GetPtr() > _sp2->GetPtr(); }


EOS_NAMESPACE_END