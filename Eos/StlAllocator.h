#pragma once

#include "CoreDefs.h"

#include "MemoryUtils.h"
#include "Allocator.h"
#include "MemoryFunctions.h"


EOS_NAMESPACE_BEGIN


template<typename T, class Allocator>
class eosStlAllocator;


// specialize for void:
template <class Allocator>
class eosStlAllocator<void, Allocator>
{
public:
    typedef void*       pointer;
    typedef const void* const_pointer;

    // reference to void members are impossible
    typedef void        value_type;

    // A struct to rebind the allocator to another allocator of type U
    template <class U>
    struct rebind
    {
        typedef eosStlAllocator<U, Allocator> other;
    };
};


template<typename T, class Allocator>
class eosStlAllocator
{
private:
    template<typename U, typename AllocatorU>
    friend class eosStlAllocator;

public:
    // A struct to rebind the allocator to another allocator of type U
    template<typename U>
    struct rebind
    {
        typedef eosStlAllocator<U, Allocator> other;
    };

public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    eosStlAllocator(Allocator* _allocator) noexcept : m_allocator(_allocator)
    {
    }

    eosStlAllocator(eosStlAllocator const& _other) noexcept
    {
		m_allocator = _other.m_allocator;
    }

    eosStlAllocator(eosStlAllocator&& _other) noexcept
    {
		std::swap(m_allocator, _other.m_allocator);
    }

    template<typename U>
    eosStlAllocator(eosStlAllocator<U, Allocator> const& _other) noexcept
    {
		m_allocator = _other.m_allocator;
    }

    template<typename U>
    eosStlAllocator(eosStlAllocator<U, Allocator>&& _other) noexcept
    {
		m_allocator = std::forward<eosStlAllocator<U, Allocator>>(_other.m_allocator);
    }

    ~eosStlAllocator()
    {

    }

    eosStlAllocator& operator=(const eosStlAllocator& _other)
    {
        m_allocator = _other.m_allocator;
        return *this;
    }

    eosStlAllocator& operator=(eosStlAllocator&& _other)
    {
        std::swap(m_allocator, _other.m_allocator);
        return *this;
    }

    template<typename U>
    eosStlAllocator& operator=(const eosStlAllocator<U, Allocator>& _other)
    {
        m_allocator = _other.m_allocator;
        return *this;
    }

    template<typename U>
    eosStlAllocator& operator=(eosStlAllocator<U, Allocator>&& _other)
    {
        std::swap(m_allocator, _other.m_allocator);
        return *this;
    }


    // Get the address of a reference
    pointer address(reference _address) const
    {
        return &_address;
    }
    // Get the const address of a const reference
    const_pointer address(const_reference _address) const
    {
        return &_address;
    }

    // Allocate memory
    pointer allocate(size_type cnt, const void* hint = 0)
    {
        eosAssertDialog(m_allocator);

        const size_type size = cnt * sizeof(value_type);

        //new ((Allocator)->Allocate(sizeof(Type), Alignment, EOS_MEMORY_SOURCE_ALLOCATION_INFO)) Type(__VA_ARGS__)
        void* pAllocation = m_allocator->Allocate(size, __alignof(value_type), EOS_MEMORY_SOURCE_ALLOCATION_INFO);

        eosAssertReturnValue(pAllocation != nullptr, nullptr, "StlAllocator failed to allocate memory.");

        return (pointer)pAllocation;
    }

    // Deallocate memory
    void deallocate(void* p, size_type cnt)
    {
        eosAssertDialog(m_allocator);

        m_allocator->Free(p);
    }

    // Call constructor
    void construct(pointer p, const_reference val)
    {
        // Placement new
        new ((pointer)p) T(val);
    }

    // Call constructor
    void construct(pointer p, T&& val)
    {
        new ((pointer)p) T(std::forward<T>(val));
    }

    // Call constructor
    template <class U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    // Call destructor
    void destroy(pointer p)
    {
        p->~T();
    }

    // Call destructor
    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

    // Get the max allocation size
    size_type max_size() const
    {
#ifdef max
#undef max
#endif
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    void reset()
    {
        eosAssertDialog(m_allocator);

        m_allocator->Reset();
    }

    void purge()
    {
        eosAssertDialog(m_allocator);

        m_allocator->Purge();
    }

    const Allocator& getAllocator() const
    {
        return *m_allocator;
    }

protected:
    eosStlAllocator() = default;

protected:
    Allocator* m_allocator;
};


// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, typename Allocator1, typename Allocator2>
inline bool operator==(const eosStlAllocator<T1, Allocator1>& a, const eosStlAllocator<T2, Allocator2>& b)
{
    return &a.getAllocator() == &b.getAllocator();
}

// Another allocator of the another type cannot deallocate from this one
template<typename T, typename Allocator, typename Other>
inline bool operator==(const eosStlAllocator<T, Allocator>&, const Other&)
{
    return false;
}

// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, typename Allocator1, typename Allocator2>
inline bool operator!=(const eosStlAllocator<T1, Allocator1>& a, const eosStlAllocator<T2, Allocator2>& b)
{
    return &a.getAllocator() != &b.getAllocator();
}

// Another allocator of the another type cannot deallocate from this one
template<typename T, typename Allocator, typename Other>
inline bool operator!=(const eosStlAllocator<T, Allocator>&, const Other&)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
//                  HASH
///////////////////////////////////////////////////////////////////////////

template<typename T, typename U>
class hash_pair
{
public:
    eosSize operator()(const std::pair<T, U>& _hash) const
    {
        eosSize a = static_cast<eosSize>(std::hash<T>()(_hash.first));
        eosSize b = static_cast<eosSize>(std::hash<U>()(_hash.second));
        return a ^ b;
    }
};


EOS_NAMESPACE_END
