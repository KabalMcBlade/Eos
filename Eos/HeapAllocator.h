#pragma once


#include "CoreDefs.h"

#include "VirtualMemory.h"
#include "MemoryUtils.h"
#include "MemoryBuddyAllocator.h"


EOS_NAMESPACE_BEGIN


template<eosSize MaxAlignment, eosU32 Level>
class eosHeapAllocator
{
public:
	eosHeapAllocator(eosSize _size, eosSize _offset) : 
		m_owner(EOwnerType_Malloc), 
		m_virtualStart(malloc(_size)),
		m_virtualEnd((void*)((eosUPtr)m_virtualStart + _size)),
		m_physicalEnd(m_virtualEnd),
		m_freeList(HeapFreelist(m_virtualStart, m_virtualEnd, _offset/* + kAllocationHeaderSize*/))
	{
	}

// 	eosHeapAllocator(eosSize _startSize, eosSize _maxSize, eosSize _offset) : m_owner(EOwnerType_VirtualAlloc)
// 	{
// 	}

	eosHeapAllocator(void* _start, void* _end, eosSize _offset) : 
		m_owner(EOwnerType_None),
		m_virtualStart(_start),
		m_virtualEnd(_end),
		m_physicalEnd(_end),
		m_freeList(HeapFreelist(m_virtualStart, m_virtualEnd, _offset/* + kAllocationHeaderSize*/))
	{
	}

	~eosHeapAllocator()
	{
		switch (m_owner)
		{
		case EOwnerType_Malloc:
			free(m_virtualStart);
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

		eosAssertReturnValue(_alignment <= MaxAlignment, nullptr, "Alignment must be lesser or equal the Max Alignment, Alignment = %zd, Max Alignment = %zd", _alignment, MaxAlignment);

		void* ptr = m_freeList.Alloc(_size, _alignment, _offset);

		eosAssertReturnValue(ptr, nullptr, "Heap Allocator out of Memory");

		return ptr;
	}

	EOS_INLINE void Free(void* _ptr, eosSize /*_size*/)
	{
		m_freeList.Free(_ptr);
	}

	EOS_INLINE void Reset()
	{
		m_freeList = HeapFreelist(m_virtualStart, m_virtualEnd, _offset);
	}

	// Is a fake Purge (expensiveness having real purge)
	EOS_INLINE void Purge()
	{
		Reset();
	}

	EOS_INLINE eosSize GetTotalUsedSize()  const
	{
		return GetPhysicalSize();
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
		EOwnerType_Malloc
	};

	using HeapFreelist = eosMemoryBuddyAllocator<MaxAlignment, Level>;

	void* m_virtualStart;
	void* m_virtualEnd;
	void* m_physicalEnd;

	HeapFreelist m_freeList;

	EOwnerType m_owner;

	eosSize m_growSize;
};



EOS_NAMESPACE_END