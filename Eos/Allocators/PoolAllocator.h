#pragma once

#include "../Core/BasicTypes.h"
#include "../Core/Assertions.h"
#include "../Core/NumberUtils.h"
#include "../Core/PointerUtils.h"
#include "../DataStructures/StackLinkedList.h"

#include "../MemoryAllocationPolicy.h"



EOS_NAMESPACE_BEGIN


template<size ChunkSize, size Alignment>
class PoolAllocator
{
public:
	// is a pool allocator, array makes no sense, it is allocated already at construction time the full list and you get one by one
	static constexpr bool kAllowedAllocationArray = false;

	PoolAllocator(void* _start, void* _end, size _headerSize, size _footerSize) : m_usedMemory(0)
	{
		eosAssertReturnVoid(_start != nullptr, "start pointer is invalid");
		eosAssertReturnVoid(_end != nullptr, "end pointer is invalid");
		eosAssertReturnVoid(_start < _end, "end is greater than start");

		m_start = (uintPtr)_start;
		m_end = (uintPtr)_end;
		m_headerSize = _headerSize;
		m_footerSize = _footerSize;

		m_fullChunkSize = (ChunkSize + m_headerSize + m_footerSize);

		Reset();
	}

	~PoolAllocator()
	{

	}

	EOS_INLINE void* Allocate(size _size, size _alignment, size /*_headerSize*/, size /*_footerSize*/)
	{
		eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
		eosAssertReturnValue(_size == m_fullChunkSize, nullptr, "Allocation size must be equal to chunk size");
		eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
		eosAssertReturnValue(_alignment == Alignment, nullptr, "Alignment must be equal to alignment set");
		eosAssertReturnValue(CoreUtils::IsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");

		Node* buffer = m_freeList.Pop();

		eosAssertReturnValue(buffer != nullptr, nullptr, "The allocator is full");

		m_usedMemory += _size;

		return (void*)buffer;
	}

	EOS_INLINE void Free(void* _ptr, size _size)
	{
		m_usedMemory -= _size;

		m_freeList.Push((Node*)_ptr);
	}

	EOS_INLINE void Reset()
	{
		m_usedMemory = 0;
		m_current = m_start;

		m_chunkCount = static_cast<uint32>(GetTotalMemory() / m_fullChunkSize);
		uint32 i = 0;
		while (i < m_chunkCount)
		{
			uintPtr address = CoreUtils::AlignTop(m_current + m_headerSize, Alignment) - m_headerSize;
			m_freeList.Push((Node*)address);
			m_current += m_fullChunkSize;
			++i;
		}
	}

	EOS_INLINE size GetUsedMemory()  const
	{
		return m_current - m_start;
	}

	EOS_INLINE size GetTotalMemory() const
	{
		return m_end - m_start;
	}

private:
	struct  FreeHeader {};
	using Node = typename StackLinkedList<FreeHeader>::Node;
	StackLinkedList<FreeHeader> m_freeList;

	uintPtr m_start;
	uintPtr m_end;
	uintPtr m_current;

	uint32 m_chunkCount;

	size m_headerSize;
	size m_footerSize;
	size m_usedMemory;
	size m_fullChunkSize;
};


template<size ChunkSize, size Alignment>
using PoolAllocationPolicy = AllocationPolicy<PoolAllocator<ChunkSize, Alignment>, AllocationHeader>;

EOS_NAMESPACE_END