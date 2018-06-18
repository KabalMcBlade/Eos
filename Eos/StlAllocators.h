#pragma once

#include "CoreDefs.h"

#include "MemoryManager.h"

// to remove error C2589 using std::min / max
#define NOMINMAX


#ifdef max
#undef max
#endif //

#ifdef min
#undef min
#endif //


#define EOS_STL_ALIGNMENT           EOS_MEMORY_ALIGNMENT_SIZE

EOS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
//    Traits that describes an object T
///////////////////////////////////////////////////////////////////////////

// a standard object trait
template<typename T>
class ObjectTraits {
public:
    //    convert an ObjectTraits<T> to ObjectTraits<U>
    template<typename U>
    struct rebind {
        typedef ObjectTraits<U> other;
    };

public:
    EOS_INLINE explicit ObjectTraits() {}
    EOS_INLINE ~ObjectTraits() {}
    EOS_INLINE explicit ObjectTraits(ObjectTraits  const&) {}
    template <typename U>
    EOS_INLINE explicit ObjectTraits(ObjectTraits<U> const&) {}

    //    address
    EOS_INLINE T* address(T& r) { return &r; }
    EOS_INLINE T const* address(T const& r) { return &r; }

    // displacement allocator only for STL purpose
    EOS_INLINE static void construct(T* p, const T& t)  { new(p) T(t); }
    EOS_INLINE static void destroy(T* p) {  p->~T(); }
};    //    end of class ObjectTraits

///////////////////////////////////////////////////////////////////////////
//    class HeapAllocPolicy
///////////////////////////////////////////////////////////////////////////

//    a standard allocation policy using the free store
template<typename T>
class HeapAllocPolicy {
public:
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public:
    //    convert an HeapAllocPolicy<T> to HeapAllocPolicy<U>
    template<typename U>
    struct rebind {
        typedef HeapAllocPolicy<U> other;
    };

public:
    EOS_INLINE explicit HeapAllocPolicy() {}
    EOS_INLINE ~HeapAllocPolicy() {}
    EOS_INLINE explicit HeapAllocPolicy(HeapAllocPolicy const&) {}
    template <typename U>
    EOS_INLINE explicit HeapAllocPolicy(HeapAllocPolicy<U> const&) {}

    //    memory allocation
    EOS_INLINE pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) 
    { 
        return static_cast<pointer>(eosNewRaw(cnt * sizeof(T), EOS_STL_ALIGNMENT));
    }
    EOS_INLINE void deallocate(pointer p, size_type)
    { 
        eosDeleteRaw(p);
    }

    //    size
    EOS_INLINE size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
};    //    end of class HeapAllocPolicy

//    determines if memory from another allocator can be deallocated from this one
template<typename T, typename T2>
EOS_INLINE bool operator==(HeapAllocPolicy<T> const&, HeapAllocPolicy<T2> const&)
{
    return true;
}
template<typename T, typename OtherAllocator>
EOS_INLINE bool operator==(HeapAllocPolicy<T> const&, OtherAllocator const&)
{
    return false;
}



///////////////////////////////////////////////////////////////////////////
//    class StackAllocPolicy
///////////////////////////////////////////////////////////////////////////

//    a fixed size array allocation policy
//    should be used with vector only
template<typename T>
class StackAllocPolicy {
public:
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public:
    //    convert an StackAllocPolicy<T> to StackAllocPolicy<U>
    template<typename U>
    struct rebind {
        typedef StackAllocPolicy<U> other;
    };

public:
    EOS_INLINE explicit StackAllocPolicy() {}
    EOS_INLINE ~StackAllocPolicy() {}
    EOS_INLINE explicit StackAllocPolicy(StackAllocPolicy const&) {}
    template <typename U>
    EOS_INLINE explicit StackAllocPolicy(StackAllocPolicy<U> const&) {}

    //    memory allocation
    EOS_INLINE pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) 
    {
        return static_cast<pointer>(eosNewRawStack(cnt * sizeof(T), EOS_STL_ALIGNMENT));
    }
    EOS_INLINE void deallocate(pointer p, size_type)
    {
        eosDeleteRawStack(p);
    }

    //    size
    EOS_INLINE size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
};    //    end of class StackAllocPolicy

//    determines if memory from another allocator can be deallocated from this one
template<typename T, typename T2>
EOS_INLINE bool operator==(StackAllocPolicy<T> const&, StackAllocPolicy<T2> const&) 
{
    return false;
}
template<typename T, typename OtherAllocator>
EOS_INLINE bool operator==(StackAllocPolicy<T> const&, OtherAllocator const&) 
{
    return false;
}



///////////////////////////////////////////////////////////////////////////
//    class LinearAllocPolicy
///////////////////////////////////////////////////////////////////////////

template<typename T>
class LinearAllocPolicy {
public:
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public:
    //    convert an LinearAllocPolicy<T> to LinearAllocPolicy<U>
    template<typename U>
    struct rebind {
        typedef LinearAllocPolicy<U> other;
    };

public:
    EOS_INLINE explicit LinearAllocPolicy() {}
    EOS_INLINE ~LinearAllocPolicy() {}
    EOS_INLINE explicit LinearAllocPolicy(LinearAllocPolicy const&) {}
    template <typename U>
    EOS_INLINE explicit LinearAllocPolicy(LinearAllocPolicy<U> const&) {}

    //    memory allocation
    EOS_INLINE pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0)
    {
        return static_cast<pointer>(eosNewRawLinear(cnt * sizeof(T), EOS_STL_ALIGNMENT));
    }
    EOS_INLINE void deallocate(pointer p, size_type) 
    {
        eosDeleteRawLinear(p);
    }

    //    size
    EOS_INLINE size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
};    //    end of class LinearAllocPolicy

    //    determines if memory from another allocator can be deallocated from this one
template<typename T, typename T2>
EOS_INLINE bool operator==(LinearAllocPolicy<T> const&, LinearAllocPolicy<T2> const&)
{
    return true;
}
template<typename T, typename OtherAllocator>
EOS_INLINE bool operator==(LinearAllocPolicy<T> const&, OtherAllocator const&) 
{
    return false;
}


///////////////////////////////////////////////////////////////////////////
//    class StlAllocator
///////////////////////////////////////////////////////////////////////////

//    Policy driven allocator object
template<typename T, typename Policy = HeapAllocPolicy<T>, typename Traits = ObjectTraits<T> >
class StlAllocator : public Policy, public Traits {
private:
    typedef Policy AllocationPolicy;
    typedef Traits TTraits;

public:
    typedef typename AllocationPolicy::size_type size_type;
    typedef typename AllocationPolicy::difference_type difference_type;
    typedef typename AllocationPolicy::pointer pointer;
    typedef typename AllocationPolicy::const_pointer const_pointer;
    typedef typename AllocationPolicy::reference reference;
    typedef typename AllocationPolicy::const_reference const_reference;
    typedef typename AllocationPolicy::value_type value_type;
    
    
public:
    template<typename U>
    struct rebind
    {
        typedef StlAllocator<U, typename AllocationPolicy::template rebind<U>::other, typename Traits::template rebind<U>::other > other;
    };

public:
    EOS_INLINE explicit StlAllocator() {}
    EOS_INLINE ~StlAllocator() {}
    EOS_INLINE StlAllocator(StlAllocator const& rhs) :Traits(rhs), Policy(rhs) {}
    template <typename U>
    EOS_INLINE explicit StlAllocator(StlAllocator<U> const&) {}
    template <typename U, typename P, typename T2>
    EOS_INLINE StlAllocator(StlAllocator<U, P, T2> const& rhs) : Traits(rhs), Policy(rhs) {}

    //    memory allocation
    EOS_INLINE pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer hint = 0)
    {
        return AllocationPolicy::allocate(cnt, hint);
    }
    EOS_INLINE void deallocate(pointer p, size_type cnt) 
    {
        AllocationPolicy::deallocate(p, cnt);
    }
};    //    end of class StlAllocator


//    determines if memory from another allocator can be deallocated from this one
template<typename T, typename P, typename Tr>
EOS_INLINE bool operator==(StlAllocator<T, P, Tr> const& lhs, StlAllocator<T, P, Tr> const& rhs) 
{
    return operator==(static_cast<P const&>(lhs), static_cast<P const&>(rhs));
}
template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
EOS_INLINE bool operator==(StlAllocator<T, P, Tr> const& lhs, StlAllocator<T2, P2, Tr2> const& rhs) 
{
    return operator==(static_cast<P const&>(lhs), static_cast<P2 const&>(rhs));
}
template<typename T, typename P, typename Tr, typename OtherAllocator>
EOS_INLINE bool operator==(StlAllocator<T, P, Tr> const& lhs, OtherAllocator const& rhs) 
{
    return operator==(static_cast<P const&>(lhs), rhs);
}
template<typename T, typename P, typename Tr>
EOS_INLINE bool operator!=(StlAllocator<T, P, Tr> const& lhs, StlAllocator<T, P, Tr> const& rhs)
{
    return !operator==(lhs, rhs);
}
template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
EOS_INLINE bool operator!=(StlAllocator<T, P, Tr> const& lhs, StlAllocator<T2, P2, Tr2> const& rhs) 
{
    return !operator==(lhs, rhs);
}
template<typename T, typename P, typename Tr, typename OtherAllocator>
EOS_INLINE bool operator!=(StlAllocator<T, P, Tr> const& lhs, OtherAllocator const& rhs) 
{
    return !operator==(lhs, rhs);
}

EOS_NAMESPACE_END
