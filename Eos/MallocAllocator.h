#pragma once


#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN

// Special allocator:
// This allocator is an actual malloc allocator, and using 2 on 3 of the constructor but altering the functionality:
// eosMallocAllocator(eosSize _size, eosSize _offset)  -> using the _size as max size
// eosMallocAllocator(eosSize _startSize, eosSize _maxSize, eosSize _offset) -> the _startSize does not matter
template<eosSize MinAlignment>
class eosMallocAllocator
{
public:
	eosMallocAllocator(eosSize _size, eosSize _offset) : m_currentSize(0), m_maxSize(_size)
	{
	}

	eosMallocAllocator(eosSize _startSize, eosSize _maxSize, eosSize _offset) : m_currentSize(0), m_maxSize(_maxSize)
	{
	}

// 	eosMallocAllocator(void* _start, void* _end, eosSize _offset)
// 	{
// 	}

	~eosMallocAllocator()
	{
	}

	EOS_INLINE void* Allocate(eosSize _size, eosSize _alignment, eosSize _offset)
	{
		eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
		eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
		eosAssertReturnValue(eosIsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");
		eosAssertReturnValue(m_currentSize < m_maxSize, nullptr, "Memory allocated out of bound");

		_size += kAllocationHeaderSize;

		_alignment = _alignment > MinAlignment ? _alignment : MinAlignment;

		void* addr = malloc(_size + _alignment + sizeof(eosUPtr));

		eosUPtr aligned_addr = eosPointerUtils::AlignTop(reinterpret_cast<eosUPtr>(addr) + _offset /*+ sizeof(eosUPtr)*/, _alignment) - (_offset /*+ sizeof(eosUPtr)*/);

		// store the original address
		*(eosUPtr*)(aligned_addr - sizeof(eosUPtr)) = (eosUPtr)addr;

		return (void*)aligned_addr;
	}

	EOS_INLINE void Free(void* _ptr, eosSize _size)
	{
		eosUPtr orig_ptr = *(eosUPtr*)((eosUPtr)_ptr - sizeof(eosUPtr));
		free((void*)orig_ptr);
	}

	EOS_INLINE void Reset()
	{
		// Nothing
	}

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
		return m_currentSize;
	}

	EOS_INLINE eosSize GetVirtualSize() const
	{
		return m_maxSize;
	}

private:
	using Header = eosU8;

	static constexpr eosSize kAllocationHeaderSize = sizeof(Header);
	static_assert(kAllocationHeaderSize >= 1, "Header has wrong size.");

	eosSize m_maxSize;
	eosSize m_currentSize;
};



EOS_NAMESPACE_END