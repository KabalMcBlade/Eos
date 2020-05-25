#pragma once


#include <limits>

#include "../Core/BasicTypes.h"
#include "../Core/Assertions.h"
#include "../Core/NumberUtils.h"
#include "../Core/PointerUtils.h"
#include "../DataStructures/LinkedList.h"

#include "../MemoryAllocationPolicy.h"


EOS_OPTIMIZATION_OFF

EOS_NAMESPACE_BEGIN


enum EFreeListSearch
{
	EFreeListSearch_First,
	EFreeListSearch_Best
};


template<EFreeListSearch Search>
class FreeListAllocator
{
private:
	struct  Header
	{
		size m_blockSize;
	};
	struct AllocationHeader 
	{
		size m_blockSize;
		uint8 m_padding;
	};

	using Node = typename LinkedList<Header>::Node;

	const size kAllocationHeaderSize = sizeof(AllocationHeader);

public:
	static constexpr bool kAllowedAllocationArray = true;


	FreeListAllocator(void* _start, void* _end, size /*_headerSize*/, size /*_footerSize*/) : m_usedMemory(0)
	{
		eosAssertReturnVoid(_start != nullptr, "start pointer is invalid");
		eosAssertReturnVoid(_end != nullptr, "end pointer is invalid");
		eosAssertReturnVoid(_start < _end, "end is greater than start");

		m_start = (uintPtr)_start;
		m_end = (uintPtr)_end;

		Reset();
	}

	~FreeListAllocator()
	{

	}

	EOS_INLINE void* Allocate(size _size, size _alignment, size _headerSize, size /*_footerSize*/)
	{
		eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
		eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
		eosAssertReturnValue(CoreUtils::IsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");

		Node* nodeFound = nullptr;
		Node* prevNode = nullptr;
		size padding = 0;
		Find(_size, _alignment, _headerSize, padding, prevNode, nodeFound);

		//nodeFound = (Node*)CoreUtils::AlignTop((uintPtr)nodeFound + _headerSize, _alignment) - _headerSize;

		eosAssertReturnValue(nodeFound != nullptr, nullptr, "Memory over, please resize the allocator!");

		//const size alignmentPadding = padding - kAllocationHeaderSize;
		const size requiredSize = _size + padding;

		const size left = nodeFound->m_data.m_blockSize - requiredSize;

		if (left > 0)
		{
			Node* newFreeNode = (Node*)((size)nodeFound + kAllocationHeaderSize + requiredSize);
			newFreeNode->m_data.m_blockSize = left;
			m_freeList.Insert(nodeFound, newFreeNode);
		}
		m_freeList.Remove(prevNode, nodeFound);

		const size headerAddress = (size)(nodeFound) + padding/*alignmentPadding*/;

		const size dataAddress = headerAddress + kAllocationHeaderSize;
		((FreeListAllocator::AllocationHeader *) headerAddress)->m_blockSize = requiredSize;
		((FreeListAllocator::AllocationHeader *) headerAddress)->m_padding = static_cast<uint8>(padding/*alignmentPadding*/);

		m_usedMemory += requiredSize;

		return (void*)dataAddress;
	}

	EOS_INLINE void Free(void* _ptr, size /*_size*/)
	{
		const size currentAddress = (size)_ptr;
		const size headerAddress = currentAddress - sizeof(FreeListAllocator::AllocationHeader);
		const FreeListAllocator::AllocationHeader* allocationHeader { (FreeListAllocator::AllocationHeader*) headerAddress };

		Node* freeNode = (Node*)(headerAddress);
		freeNode->m_data.m_blockSize = allocationHeader->m_blockSize + allocationHeader->m_padding;
		freeNode->m_next = nullptr;

		Node* it = m_freeList.GetHead();
		Node* itPrev = nullptr;
		while (it != nullptr)
		{
			if (_ptr < it) 
			{
				m_freeList.Insert(itPrev, freeNode);
				break;
			}
			itPrev = it;
			it = it->m_next;
		}

		m_usedMemory -= freeNode->m_data.m_blockSize;

		Coalescence(itPrev, freeNode);
	}

	EOS_INLINE void Reset()
	{
		m_usedMemory = 0;
		Node* firstNode = (Node*)m_start;
		firstNode->m_data.m_blockSize = GetTotalMemory();
		firstNode->m_next = nullptr;
		m_freeList.SetHead(nullptr);
		m_freeList.Push(firstNode);
	}

	EOS_INLINE size GetUsedMemory()  const
	{
		return m_usedMemory;
	}

	EOS_INLINE size GetTotalMemory() const
	{
		return m_end - m_start;
	}

private:
	void Coalescence(Node* _prev, Node* _block)
	{
		if (_block->m_next != nullptr && (size)_block + _block->m_data.m_blockSize == (size)_block->m_next)
		{
			_block->m_data.m_blockSize += _block->m_next->m_data.m_blockSize;
			m_freeList.Remove(_block, _block->m_next);
		}

		if (_prev != nullptr && (size)_prev + _prev->m_data.m_blockSize == (size)_block)
		{
			_prev->m_data.m_blockSize += _block->m_data.m_blockSize;
			m_freeList.Remove(_prev, _block);
		}
	}

	void Find(const size _size, const size _alignment, size _headerSize, size& _padding, Node*& _prev, Node*& _found)
	{
		// look at the specialization
	}
	
private:
	LinkedList<Header> m_freeList;

	uintPtr m_start;
	uintPtr m_end;

	size m_usedMemory;
};

template <>
void FreeListAllocator<EFreeListSearch::EFreeListSearch_First>::Find(const size _size, const size _alignment, size _headerSize, size& _padding, Node*& _prev, Node*& _found)
{
	Node* it = m_freeList.GetHead();
	Node* itPrev = nullptr;

	while (it != nullptr)
	{
		//_padding = CoreUtils::AlignTopAmount((uintPtr)it + kAllocationHeaderSize, _alignment) + kAllocationHeaderSize;

		const uintPtr curr = (uintPtr)it + kAllocationHeaderSize;
		const uintPtr temp = CoreUtils::AlignTop(curr + _headerSize, _alignment) - _headerSize;
		_padding = (temp - curr);

		const size requiredSpace = _size + _padding;

		if (it->m_data.m_blockSize >= requiredSpace) 
		{
			break;
		}

		itPrev = it;
		it = it->m_next;
	}

	_prev = itPrev;
	_found = it;
}

template <>
void FreeListAllocator<EFreeListSearch::EFreeListSearch_Best>::Find(const size _size, const size _alignment, size _headerSize, size& _padding, Node*& _prev, Node*& _found)
{
#ifdef max
#undef max
#endif
	size smallestDiff = std::numeric_limits<size>::max();

	Node* bestBlock = nullptr;
	Node* it = m_freeList.GetHead();
	Node* itPrev = nullptr;

	while (it != nullptr) 
	{
		//_padding = CoreUtils::AlignTopAmount((uintPtr)it + kAllocationHeaderSize, _alignment) + kAllocationHeaderSize;

		const uintPtr curr = (uintPtr)it + kAllocationHeaderSize;
		const uintPtr temp = CoreUtils::AlignTop(curr + _headerSize, _alignment) - _headerSize;
		_padding = (temp - curr);

		const size requiredSpace = _size + _padding;

		if (it->m_data.m_blockSize >= requiredSpace && ( (it->m_data.m_blockSize - requiredSpace) < smallestDiff))
		{
			bestBlock = it;
		}
		itPrev = it;
		it = it->m_next;
	}

	_prev = it;
	_found = bestBlock;
}



using FreeListBestSearchAllocationPolicy = AllocationPolicy<FreeListAllocator<EFreeListSearch::EFreeListSearch_Best>, AllocationHeader>;
using FreeListFirstSearchAllocationPolicy = AllocationPolicy<FreeListAllocator<EFreeListSearch::EFreeListSearch_First>, AllocationHeader>;

EOS_NAMESPACE_END