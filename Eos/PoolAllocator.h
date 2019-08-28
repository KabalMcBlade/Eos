#pragma once


#include "CoreDefs.h"

#include "VirtualMemory.h"
#include "MemoryUtils.h"
#include "MemoryFreeList.h"


EOS_NAMESPACE_BEGIN

template<eosSize MaxElementSize, eosSize MaxAlignment, eosBool CanGrow, eosSize GrowSize>
class eosPoolAllocator
{
public:
    eosPoolAllocator(eosSize _size, eosSize _offset) : m_owner(EOwnerType_Malloc), m_maxElementSize(kMaxElementSize + MaxAlignment + kAllocationHeaderSize), m_allocateosCounter(0)
    {
        eosAssertDialog(_size > 0);
        eosAssertDialog(!CanGrow);      // With this constructor the allocator CANNOT grow

         m_virtualStart = malloc(_size);
         m_virtualEnd = (void*)((eosUPtr)m_virtualStart + _size);
         m_physicalEnd = m_virtualEnd;

         _offset += kAllocationHeaderSize;

         m_freeList = PoolFreelist(m_virtualStart, m_virtualEnd, m_maxElementSize, MaxAlignment, _offset);
         m_wastedSpace = m_freeList.GetWastedSize();
    }

    eosPoolAllocator(eosSize _startSize, eosSize _maxSize, eosSize _offset) : m_owner(EOwnerType_VirtualAlloc), m_maxElementSize(kMaxElementSize + MaxAlignment + kAllocationHeaderSize), m_allocateosCounter(0)
    {
        eosAssertDialog(_startSize <= _maxSize);
        eosAssertDialog(CanGrow);      // With this constructor the allocator CAN grow

        if (_startSize == 0)
        {
            _startSize = _maxSize >> 1;
        }

        m_virtualStart = eosVirtualMemory::ReserveAddressSpace(_maxSize);
        m_virtualEnd = (void*)((eosUPtr)m_virtualStart + _maxSize);
        m_physicalEnd = (void*)((eosUPtr)m_virtualStart + _startSize);

        eosVirtualMemory::CommitPhysicalMemory(m_virtualStart, _startSize);

        _offset += kAllocationHeaderSize;

        m_freeList = PoolFreelist(m_virtualStart, m_physicalEnd, m_maxElementSize, MaxAlignment, _offset);
        m_wastedSpace = m_freeList.GetWastedSize();

        m_growSize = eosBitUtils::RoundUpToMultiple(GrowSize * m_freeList.GetSlotSize(), eosVirtualMemory::GetPageSize());

        eosAssertReturnVoid(m_growSize % eosVirtualMemory::GetPageSize() == 0 && m_growSize != 0, "m_growSize should be a multiple of page size.");
        eosAssertDialog(m_growSize % eosVirtualMemory::GetPageSize() == 0 && m_growSize != 0);    // to block execute
    }

    eosPoolAllocator(void* _start, void* _end, eosSize _offset) : m_owner(EOwnerType_None), m_maxElementSize(kMaxElementSize + MaxAlignment + kAllocationHeaderSize), m_allocateosCounter(0)
    {
        eosAssertDialog(_start);
        eosAssertDialog(_end);
        eosAssertDialog(_start < _end);
        eosAssertDialog(!CanGrow);      // With this constructor the allocator CANNOT grow

        m_virtualStart = _start;
        m_virtualEnd = _end;
        m_physicalEnd = _end;

        _offset += kAllocationHeaderSize;

        m_freeList = PoolFreelist(m_virtualStart, m_virtualEnd, m_maxElementSize, MaxAlignment, _offset);
        m_wastedSpace = m_freeList.GetWastedSize();
    }

    ~eosPoolAllocator()
    {
        switch (m_owner)
        {
        case EOwnerType_Malloc:
            free(m_virtualStart);
            break;
        case EOwnerType_VirtualAlloc:
            eosVirtualMemory::ReleaseAddressSpace(m_virtualStart);
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

        _size += kAllocationHeaderSize;

        eosAssertReturnValue(_size <= (m_maxElementSize + _offset), nullptr, "Size must be lesser or equal the Max Element Size, Size = %zd, Max Element Size = %zd", _size, m_maxElementSize);
        eosAssertReturnValue(_alignment <= MaxAlignment, nullptr, "Alignment must be lesser or equal the Max Alignment, Alignment = %zd, Max Alignment = %zd", _alignment, MaxAlignment);

        void* ptr = m_freeList.Get();
        if (!ptr)
        {
            if (CanGrow)
            {
                const eosSize neededPhysicalSize = m_growSize;
                const eosUPtr physicalEnd = reinterpret_cast<eosUPtr>(m_physicalEnd);
                const eosUPtr virtualEnd = reinterpret_cast<eosUPtr>(m_virtualEnd);

                eosAssertReturnValue(physicalEnd + neededPhysicalSize <= virtualEnd, nullptr, "PoolAllocator out of memory. Requested = %zd, End at + %zd", physicalEnd + neededPhysicalSize, virtualEnd);

                eosVirtualMemory::CommitPhysicalMemory(m_physicalEnd, neededPhysicalSize);

                void* newPhysicalEnd = (void*)(physicalEnd + neededPhysicalSize);

                m_wastedSpace -= m_freeList.GetWastedSizeAtBack();

                const eosBool canExtendFreeList = m_freeList.Extend(newPhysicalEnd);
                eosAssertReturnValue(canExtendFreeList, nullptr, "Failed to extend the Pool Free List");

                m_wastedSpace += m_freeList.GetWastedSize();

                m_physicalEnd = newPhysicalEnd;
                ptr = m_freeList.Get();

                eosAssertReturnValue(ptr, nullptr, "Failed to allocate object after growing the pool. Grow Size = %zd", m_growSize);
            }
            else
            {
                eosAssertReturnValue(false, nullptr, "PoolAllocator out of memory");
            }
        }

        eosUPtr newPtr = eosPointerUtils::AlignTop(reinterpret_cast<eosUPtr>(ptr) + _offset, _alignment) - _offset;
        const eosSize offsetSize = newPtr - reinterpret_cast<eosUPtr>(ptr);

        eosAssertReturnValue(offsetSize >> (sizeof(Header) * 8) == 0, nullptr, "offsetSize must be less that sizeof(AllocationHeaderType). offsetSize = %zd, sizeof(Header) = %zd", offsetSize, sizeof(Header));
        eosAssertReturnValue(newPtr + _size - kAllocationHeaderSize < reinterpret_cast<eosUPtr>(ptr) + (m_maxElementSize + _offset), nullptr, "Memory allocated out of bound.");

        union
        {
            void* as_void;
            Header* as_header;
            eosUPtr as_uptr;
        };
        as_uptr = newPtr;

        *(as_header) = static_cast<Header>(offsetSize);
		++as_header;

        ++m_allocateosCounter;

        return as_void;
    }

    EOS_INLINE void Free(void* _ptr, eosSize /*_size*/)
    {
        union
        {
            void* as_void;
            Header* as_header;
            eosUPtr as_uptr;
        };
        as_void = _ptr;

		--as_header;

        const eosU8 headerSize = *(as_header);
        as_uptr -= headerSize;

        m_freeList.Release(as_void);
        --m_allocateosCounter;
    }

    EOS_INLINE void Reset()
    {
        m_freeList = PoolFreelist(m_virtualStart, m_virtualEnd, m_maxElementSize, MaxAlignment, 0);
    }

    // Is a fake Purge (expensiveness having real purge)
    EOS_INLINE void Purge()
    {
        Reset();
    }

    EOS_INLINE eosSize GetTotalUsedSize()  const
    {
        return (m_allocateosCounter * m_freeList.GetSlotSize()) + m_wastedSpace;
    }

    EOS_INLINE eosSize GetPhysicalSize() const
    {
        return (eosUPtr)m_physicalEnd - (eosUPtr)m_virtualStart;
    }

    EOS_INLINE eosSize GetVirtualSize() const
    {
        return (eosUPtr)m_virtualEnd - (eosUPtr)m_virtualStart;
    }

private:
    enum EOwnerType
    {
        EOwnerType_None = 0,
        EOwnerType_Malloc,
        EOwnerType_VirtualAlloc
    };

    using PoolFreelist = eosMemoryFreeList<eosEFreelist_IndexSize::EFreelist_IndexSize_Default>;
    using Header = eosU8;

    static constexpr eosSize kAllocationHeaderSize = sizeof(Header);
    static_assert(kAllocationHeaderSize >= 1, "Header has wrong size.");

	static constexpr eosSize kMaxElementSize = eosBitUtils::RoundUpToMultiple(MaxElementSize, MaxAlignment) + kAllocationHeaderSize;

	static_assert(eosIsPowerOf2(MaxAlignment), "MaxAlignment has be be of power of 2");

    void* m_virtualStart;
    void* m_virtualEnd;
    void* m_physicalEnd;

    PoolFreelist m_freeList;

    eosSize m_maxElementSize;
    eosSize m_growSize;

    eosSize m_allocateosCounter;
    eosSize m_wastedSpace;

    EOwnerType m_owner;
};


template<eosSize MaxElementSize, eosSize MaxAlignment>
using eosPoolAllocatorNoGrowable = eosPoolAllocator<MaxElementSize, MaxAlignment, false, 0>;

template<eosSize MaxElementSize, eosSize MaxAlignment, eosSize GrowSize>
using eosPoolAllocatorGrowable = eosPoolAllocator<MaxElementSize, MaxAlignment, true, GrowSize>;

//

template<eosSize MaxElementSize, eosSize MaxAlignment>
using eosDefaultPoolNoGrowableAllocationPolicy = eosAllocationPolicy<eosPoolAllocatorNoGrowable<MaxElementSize, MaxAlignment>, eosAllocationHeaderU32>;

template<eosSize MaxElementSize, eosSize MaxAlignment, eosSize GrowSize>
using eosDefaultPoolGrowableAllocationPolicy = eosAllocationPolicy<eosPoolAllocatorGrowable<MaxElementSize, MaxAlignment, GrowSize>, eosAllocationHeaderU32>;


EOS_NAMESPACE_END