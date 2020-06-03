#pragma once

#include "../Core/BasicTypes.h"
#include "../Core/Assertions.h"
#include "../Core/NumberUtils.h"
#include "../Core/PointerUtils.h"

#include "../MemoryAllocationPolicy.h"



EOS_NAMESPACE_BEGIN


class LinearAllocator
{
public:
	static constexpr bool kAllowedAllocationArray = true;

	LinearAllocator(void* _start, void* _end, size /*_headerSize*/, size /*_footerSize*/)
	{
		eosAssertReturnVoid(_start != nullptr, "start pointer is invalid");
		eosAssertReturnVoid(_end != nullptr, "end pointer is invalid");
		eosAssertReturnVoid(_start < _end, "end is greater than start");

		m_start = (uintPtr)_start;
		m_end = (uintPtr)_end;
		m_current = m_start;
	}

	~LinearAllocator()
	{
	}


	EOS_INLINE void* Allocate(size _size, size _alignment, size _headerSize, size /*_footerSize*/)
	{
		eosAssertReturnValue(_size > 0, nullptr, "Size must be greater then 0");
		eosAssertReturnValue(_alignment > 0, nullptr, "Alignment must be greater then 0");
		eosAssertReturnValue(CoreUtils::IsPowerOf2(_alignment), nullptr, "Alignment must be power of 2");

		m_current = CoreUtils::AlignTop(m_current + _headerSize, _alignment) - _headerSize;
		void* ptr = (void*)m_current;

		m_current += _size;

		eosAssertReturnValue(ptr, nullptr, "Linear Allocator returned unexpected null");
		eosAssertReturnValue(m_current < m_end, nullptr, "Linear Allocator is out of memory");

		return ptr;
	}

	// Cannot free a linear allocator
	EOS_INLINE void Free(void* /*_ptr*/, size /*_size*/)
	{
	}

	// The linear allocator never store its own size
	EOS_INLINE size GetAllocatedSize(void* /*_ptr*/)
	{
		return 0;
	}

	EOS_INLINE void Reset()
	{
		m_current = m_start;
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
	uintPtr m_start;
	uintPtr m_end;
	uintPtr m_current;
};


using LinearAllocationPolicy = AllocationPolicy<LinearAllocator, AllocationHeader>;


EOS_NAMESPACE_END