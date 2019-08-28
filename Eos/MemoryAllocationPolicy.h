#pragma once


#include "CoreDefs.h"
#include "NonCopyable.h"

#include "MemoryAllocationHeaderPolicy.h"


EOS_NAMESPACE_BEGIN


template<typename eosAllocator, class eosHeaderPolicy>
class eosAllocationPolicy : eosNonCopyMove
{
public:
    static constexpr eosSize kHeaderSize = eosHeaderPolicy::kHeaderSize;

    eosAllocationPolicy(eosSize _size, eosSize _offset) : m_allocator(_size, _offset)
    {
    }

    eosAllocationPolicy(eosSize _startSize, eosSize _maxSize, eosSize _offset) : m_allocator(_startSize, _maxSize, _offset)
    {
    }

    eosAllocationPolicy(void* _start, void* _end, eosSize _offset) : m_allocator(_start, _end, _offset)
    {
    }

    EOS_INLINE void StoreAllocationSize(void* _ptr, eosSize _size)
    {
        m_header.StoreAllocationSize(_ptr, _size);
    }

    EOS_INLINE eosSize GetAllocationSize(void* _ptr)
    {
        return m_header.GetAllocationSize(_ptr);
    }

    EOS_INLINE void* Allocate(eosSize _size, eosSize _alignment, eosSize _count, eosSize _offset)
    {
        return m_allocator.Allocate(_size, _alignment, _count, _offset);
    }

    EOS_INLINE void Free(void* _ptr, eosSize _size)
    {
        m_allocator.Free(_ptr, _size);
    }

    EOS_INLINE eosSize GetTotalUsedSize()  const
    {
        return m_allocator.GetTotalUsedSize(); 
    }

    EOS_INLINE eosSize GetPhysicalSize() const
    {
        return m_allocator.GetPhysicalSize();
    }

    EOS_INLINE eosSize GetVirtualSize() const
    {
        return m_allocator.GetVirtualSize();
    }

private:
    eosAllocator m_allocator;
    eosHeaderPolicy m_header;
};

EOS_NAMESPACE_END