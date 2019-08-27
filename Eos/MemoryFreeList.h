#pragma once

#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN


enum class eosEFreelist_IndexSize
{
    EFreelist_IndexSize_Default = 0,
    EFreelist_IndexSize_1_Byte  = 1,
    EFreelist_IndexSize_2_Bytes = 2,
    EFreelist_IndexSize_4_Bytes = 4,
    EFreelist_IndexSize_8_Bytes = 8
};

template<eosEFreelist_IndexSize I = eosEFreelist_IndexSize::EFreelist_IndexSize_Default>
struct eosMemoryFreeListNode {};

template<> struct eosMemoryFreeListNode<eosEFreelist_IndexSize::EFreelist_IndexSize_1_Byte>   { eosU8 m_offset; };
template<> struct eosMemoryFreeListNode<eosEFreelist_IndexSize::EFreelist_IndexSize_2_Bytes>  { eosU16 m_offset; };
template<> struct eosMemoryFreeListNode<eosEFreelist_IndexSize::EFreelist_IndexSize_4_Bytes>  { eosU32 m_offset; };
template<> struct eosMemoryFreeListNode<eosEFreelist_IndexSize::EFreelist_IndexSize_8_Bytes>  { eosU64 m_offset; };
template<> struct eosMemoryFreeListNode<eosEFreelist_IndexSize::EFreelist_IndexSize_Default>  { eosUPtr m_offset; };

template<eosEFreelist_IndexSize I = eosEFreelist_IndexSize::EFreelist_IndexSize_Default>
class eosMemoryFreeList
{
public:
    static constexpr eosSize kIndexTypeSize = sizeof(eosMemoryFreeListNode<I>);

public:
    eosMemoryFreeList() :
        m_start(0),
        m_alignedStart(0),
        m_end(0),
        m_numElements(0),
        m_actualEnd(0),
        m_next(nullptr)
    {
	}

    eosMemoryFreeList(void* _start, void* _end, eosSize _elementSize, eosSize _alignment, eosSize _offset)
    {
        eosAssertDialog(_alignment > 0);

        if (_elementSize < kIndexTypeSize)
        {
            _elementSize = kIndexTypeSize;
        }

        const eosUPtr alignedStart = eosPointerUtils::AlignTop(reinterpret_cast<eosUPtr>(_start) + _offset, _alignment) - _offset;
        m_start = reinterpret_cast<eosUPtr>(_start);
        m_alignedStart = alignedStart;

        m_slotSize = eosBitUtils::RoundUpToMultiple(_elementSize, _alignment);
        eosAssertDialog(m_slotSize >= _elementSize);

        const eosSize size = reinterpret_cast<eosUPtr>(_end) - alignedStart;
        eosSize numElements = size / m_slotSize;
        m_end = m_alignedStart + size;
        m_numElements = numElements;

        if (m_numElements == 0)
        {
            m_next = nullptr;
            return;
        }

        union
        {
            void* as_void;
            eosMemoryFreeListNode<I>* as_self;
            eosUPtr as_uptr;
        };

        as_uptr = alignedStart;
        m_next = as_self;
        as_uptr += m_slotSize;

        eosMemoryFreeListNode<I>* runner = m_next;
        while(--numElements > 0)
        {
            runner->m_offset = as_uptr - m_start;
            runner = as_self;
            as_uptr += m_slotSize;
        }

        runner->m_offset = 0;
        m_actualEnd = as_uptr;
    }

    EOS_INLINE eosBool Extend(void* _end)
    {
		eosAssertReturnValue(_end, false, "Pointer must be provided");

        const eosSize previousNumElements = m_numElements;
        const eosSize size = reinterpret_cast<eosUPtr>(_end) - m_alignedStart;
        const eosSize numElements = size / m_slotSize;
        m_end = m_alignedStart + size;
        m_numElements = numElements;

        if (m_numElements == 0)
        {
            m_next = nullptr;
            return false;
        }

        union
        {
            void* as_void;
            eosMemoryFreeListNode<I>* as_self;
            eosUPtr as_uptr;
        };

        as_uptr = m_actualEnd;
        m_next = as_self;

        eosSize newNumElements = m_numElements - previousNumElements;

        eosMemoryFreeListNode<I>* runner = m_next;
        while (--newNumElements > 0)
        {
            runner->m_offset = as_uptr - m_start;
            runner = as_self;
            as_uptr += m_slotSize;
        }

        runner->m_offset = 0;
        m_actualEnd = as_uptr;

        return true;
    }

    EOS_INLINE void* Get()
    {
        if (m_next == nullptr)
        {
            return nullptr;
        }

        eosMemoryFreeListNode<I>* head = m_next;

        if (head->m_offset == 0)
        {
            m_next = nullptr;
        }
        else
        {
            m_next = reinterpret_cast<eosMemoryFreeListNode<I>*>(m_start + head->m_offset);
        }

        return head;
    }

    EOS_INLINE void Release(void* _ptr)
    {
        eosAssertDialog(_ptr);
        eosAssertDialog((eosUPtr)_ptr >= m_start);
        eosAssertDialog((eosUPtr)_ptr < m_end);

        eosAssertReturnVoid(((eosUPtr)_ptr - m_alignedStart) % m_slotSize == 0, "Pointer was not the beginning of a slot");

        eosMemoryFreeListNode<I>* head = reinterpret_cast<eosMemoryFreeListNode<I>*>(_ptr);
        if (m_next)
        {
            head->m_offset = reinterpret_cast<eosUPtr>(m_next) - m_start;
        }
        else
        {
            head->m_offset = 0;
        }
        m_next = head;
    }

    EOS_INLINE eosSize GetNumElements() const
    {
        return m_numElements;
    }

    EOS_INLINE eosSize GetSlotSize() const
    {
        return m_slotSize;
    }

    EOS_INLINE eosSize GetWastedSize()
    {
        return GetWastedSizeAtFront() + GetWastedSizeAtBack();
    }

    EOS_INLINE eosSize GetWastedSizeAtFront()
    {
        return m_alignedStart - m_start;
    }

    EOS_INLINE eosSize GetWastedSizeAtBack()
    {
        return m_end - m_actualEnd;
    }

private:
    eosUPtr m_start;
    eosUPtr m_alignedStart;
    eosUPtr m_end;
    eosUPtr m_actualEnd;
    eosSize m_numElements;
    eosSize m_slotSize;
    eosMemoryFreeListNode<I>* m_next;
};



EOS_NAMESPACE_END