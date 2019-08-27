#pragma once


#include "CoreDefs.h"

#include "VirtualMemory.h"
#include "MemoryUtils.h"

#include "StackFromBottomAllocatorPolicy.h"
#include "StackFromTopAllocatorPolicy.h"

EOS_NAMESPACE_BEGIN

template<typename eosStackAllocatorLayoutPolicy, eosBool HugeMemoryPage, eosBool CanGrow>
class eosStackAllocator
{
public:
    eosStackAllocator(eosSize _size, eosSize /*_offset*/) : m_owner(EOwnerType_Malloc), m_maxSize(0)
    {
        eosAssertDialog(_size > 0);
        eosAssertDialog(!CanGrow);      // With this constructor the allocator CANNOT grow

        void* ptr = malloc(_size);
        eosAssertDialog(ptr);

        m_stackAllocatorLayout.Init(this, reinterpret_cast<eosUPtr>(ptr), 0, _size, CanGrow);
        m_stackAllocatorLayout.Reset(this, 0, CanGrow);
    }

    eosStackAllocator(eosSize _startSize, eosSize _maxSize, eosSize /*_offset*/) : m_owner(EOwnerType_VirtualAlloc), m_maxSize(_maxSize)
    {
        eosAssertDialog(_startSize <= _maxSize);
        eosAssertDialog(CanGrow);      // With this constructor the allocator CAN grow

        m_growSize = eosVirtualMemory::GetPageSize();

        void* ptr = eosVirtualMemory::ReserveAddressSpace(_maxSize, HugeMemoryPage);
        eosAssertDialog(ptr);

        m_stackAllocatorLayout.Init(this, reinterpret_cast<eosUPtr>(ptr), _startSize, _maxSize, CanGrow);
        m_stackAllocatorLayout.Reset(this, _startSize, CanGrow);

        if (_startSize > 0)
        {
            m_stackAllocatorLayout.Grow(this, _startSize, HugeMemoryPage);
        }
    }

    eosStackAllocator(void* _start, void* _end, eosSize /*_offset*/) : m_owner(EOwnerType_None), m_maxSize(0)
    {
        eosAssertDialog(_start);
        eosAssertDialog(_end);
        eosAssertDialog(_start < _end);
        eosAssertDialog(!CanGrow);      // With this constructor the allocator CANNOT grow

        m_stackAllocatorLayout.Init(this,  reinterpret_cast<eosUPtr>(_start), 0, reinterpret_cast<eosUPtr>(_end) - reinterpret_cast<eosUPtr>(_start), CanGrow);
        m_stackAllocatorLayout.Reset(this, 0, CanGrow);
    }

    ~eosStackAllocator()
    {
        switch (m_owner)
        {
        case EOwnerType_Malloc:
            free(reinterpret_cast<void*>(m_virtualStart));
            break;
        case EOwnerType_VirtualAlloc:
            m_stackAllocatorLayout.Release(this);
            break;
        case EOwnerType_None:
        default:
            break;
        }
    }


    EOS_INLINE void* Allocate(eosSize _size, eosSize _alignment, eosSize _offset)
    {
        eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
        eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
        eosAssertReturnValue(eosIsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");

        _size += kAllocationOffset;
        //_offset += kAllocationOffset;

        const eosUPtr lastPhysicalCurrent = m_physicalCurrent;

        m_physicalCurrent = m_stackAllocatorLayout.AlignPointer(this, _size, _alignment, _offset);

        const eosU32 headerSize = m_stackAllocatorLayout.CalculateHeaderSize(this, lastPhysicalCurrent, _size);

        if (m_stackAllocatorLayout.IsOutOfMemory(this, _size))
        {
            eosAssertReturnValue(CanGrow, nullptr, "StackAllocator out of memory.");

            if (!m_stackAllocatorLayout.Grow(this, _size, HugeMemoryPage))
            {
                eosAssertReturnValue(false, nullptr, "StackAllocator out of memory");
            }
        }

        return m_stackAllocatorLayout.Allocate(this, headerSize, _size);
    }

    EOS_INLINE void Free(void* _ptr, eosSize /*_size*/)
    {
        eosAssertDialog(_ptr);
        eosAssertReturnVoid(_ptr == reinterpret_cast<void*>(m_lastPtr), "The free must rollback the stack in order!");

        m_stackAllocatorLayout.Free(this, _ptr);
    }

    EOS_INLINE void Reset()
    {
        m_stackAllocatorLayout.Reset(this, 0, CanGrow);
    }

    EOS_INLINE void Purge()
    {
        m_stackAllocatorLayout.Purge(this);
    }

    EOS_INLINE eosSize GetTotalUsedSize()  const
    {
        return m_stackAllocatorLayout.GetTotalUsedSize(this);
    }

    EOS_INLINE eosSize GetPhysicalSize() const
    {
        return m_stackAllocatorLayout.GetPhysicalSize(this);
    }

    EOS_INLINE eosSize GetVirtualSize() const
    {
        return m_stackAllocatorLayout.GetVirtualSize(this);
    }

private:

    friend class eosStackFromBottomAllocatorPolicy;
    friend class eosStackFromTopAllocatorPolicy;

    enum EOwnerType
    {
        EOwnerType_None = 0,
        EOwnerType_Malloc,
        EOwnerType_VirtualAlloc
    };

    static constexpr eosSize kAllocationOffset = sizeof(eosU64);
    static_assert(kAllocationOffset == 8, "Allocation offset has wrong size.");

    eosStackAllocatorLayoutPolicy m_stackAllocatorLayout;
    eosUPtr m_virtualStart;
    eosUPtr m_virtualEnd;
    eosUPtr m_physicalEnd;
    eosUPtr m_physicalCurrent;
    eosUPtr m_growSize;
    eosUPtr m_lastPtr;

    eosSize m_maxSize;

    EOwnerType m_owner;
};


using eosStackAllocatorBottomNonGrowable = eosStackAllocator<eosStackFromBottomAllocatorPolicy, false, false>;
using eosStackAllocatorBottomGrowable = eosStackAllocator<eosStackFromBottomAllocatorPolicy, false, true>;

using eosStackAllocatorTopNonGrowable = eosStackAllocator<eosStackFromTopAllocatorPolicy, false, false>;
using eosStackAllocatorTopGrowable = eosStackAllocator<eosStackFromTopAllocatorPolicy, false, true>;

using eosStandardStackAllocator = eosStackAllocatorTopNonGrowable;


EOS_NAMESPACE_END