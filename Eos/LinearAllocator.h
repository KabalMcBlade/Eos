#pragma once


#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN


class eosLinearAllocator
{
public:
    eosLinearAllocator(eosSize _size, eosSize /*_offset*/) : m_owner(true)
    {
        m_start = (eosUPtr)malloc(_size);
        m_end = static_cast<eosUPtr>(m_start) + _size;
        m_current = m_start;
    }

//     eosLinearAllocator(eosSize _startSize, eosSize _maxSize, eosSize /*_offset*/)
//     {
//     }

    eosLinearAllocator(void* _start, void* _end, eosSize /*_offset*/) : m_owner(false)
    {
        eosAssertDialog(_start);
        eosAssertDialog(_end);
        eosAssertDialog(_start < _end);

        m_start = (eosUPtr)_start;
        m_end = (eosUPtr)_end;
        m_current = m_start;
    }

    ~eosLinearAllocator()
    {
        if (m_owner)
        {
            free((void*)m_start);
        }
    }


    EOS_INLINE void* Allocate(eosSize _size, eosSize _alignment, eosSize _count, eosSize _offset)
    {
        eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
        eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
        eosAssertReturnValue(eosIsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");
  
        m_current = eosPointerUtils::AlignTop(m_current + _offset, _alignment) - _offset;
        void* ptr = (void*)m_current;

        m_current += _size;

        eosAssertReturnValue(ptr, nullptr, "Linear Allocator returned unexpected null");
        eosAssertReturnValue(m_current < m_end, nullptr, "Linear Allocator is out of memory");

        return ptr;
    }

    EOS_INLINE void Free(void* /*_ptr*/, eosSize /*_size*/)
    {
        //eosAssertReturnVoid(false, "Linear Allocator cannot free memory");
    }

    EOS_INLINE void Reset() 
    {
        m_current = m_start;
    }

    // is a fake Purge
    EOS_INLINE void Purge()
    {
        Reset();
    }

    EOS_INLINE eosSize GetTotalUsedSize()  const
    {
        return m_current - m_start;
    }

    EOS_INLINE eosSize GetPhysicalSize() const
    {
        return m_end - m_start;
    }

    EOS_INLINE eosSize GetVirtualSize() const
    {
        return GetPhysicalSize();
    }

private:
    eosUPtr m_start;
    eosUPtr m_end;
    eosUPtr m_current;
    eosBool m_owner;
};


using eosDefaultLinearAllocationPolicy = eosAllocationPolicy<eosLinearAllocator, eosAllocationHeaderU32>;


EOS_NAMESPACE_END