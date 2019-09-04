#pragma once


#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN

// Special allocator:
// This allocator is an actual malloc allocator, and using 2 on 3 of the constructor but altering the functionality:
// eosMallocAllocator(eosSize _size, eosSize _offset)  -> using the _size as max size
// eosMallocAllocator(eosSize _startSize, eosSize _maxSize, eosSize _offset) -> the _startSize does not matter
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

	EOS_INLINE void* Allocate(eosSize _size, eosSize _alignment, eosSize _count, eosSize _offset)
	{
		eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
		eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
		eosAssertReturnValue(eosIsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");
		eosAssertReturnValue(m_currentSize < m_maxSize, nullptr, "Memory allocated out of bound");

        _size += kAllocationHeaderSize; // need to provide sufficient space to align the pointer in any alignment with offset
        void* addr = malloc(_size);
        
		eosUPtr newPtr = eosPointerUtils::AlignTop(reinterpret_cast<eosUPtr>(addr) + kAllocationHeaderSize + _offset, _alignment) - _offset;
		const eosSize offsetSize = newPtr - reinterpret_cast<eosUPtr>(addr);

		union
		{
			void* as_void;
			Header* as_header;
			eosUPtr as_uptr;
		};
		as_uptr = newPtr;

		*(as_header - 1) = static_cast<Header>(offsetSize);

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
		
		const Header headerSize = *(as_header - 1);
		as_uptr -= headerSize;

		free(as_void);
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

using eosDefaultMallocAllocationPolicy = eosAllocationPolicy<eosMallocAllocator, eosAllocationHeaderU32>;


EOS_NAMESPACE_END