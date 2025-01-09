// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryAllocationPolicy.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "MemoryHeaderPolicy.h"


EOS_NAMESPACE_BEGIN


template<typename ActualAllocator, class HeaderPolicy>
class AllocationPolicy
{
public:
	static constexpr size kHeaderSize = HeaderPolicy::kHeaderSize;
	static constexpr bool kAllowedAllocationArray = ActualAllocator::kAllowedAllocationArray;

	AllocationPolicy(void* _start, void* _end, size _headerSize, size _footerSize) : m_allocator(_start, _end, _headerSize, _footerSize)
	{
	}

	EOS_INLINE void StoreSize(void* _ptr, size _size)
	{
		m_header.StoreSize(_ptr, _size);
	}

	EOS_INLINE size GetSize(void* _ptr)
	{
		return m_header.GetSize(_ptr);
	}

	EOS_INLINE void* Allocate(size _size, size _alignment, size _headerSize, size _footerSize)
	{
		return m_allocator.Allocate(_size, _alignment, _headerSize, _footerSize);
	}

	EOS_INLINE size GetAllocatedSize(void* _ptr)
	{
		return m_allocator.GetAllocatedSize(_ptr);
	}

	EOS_INLINE void Free(void* _ptr, size _size)
	{
		m_allocator.Free(_ptr, _size);
	}

	EOS_INLINE size GetUsedMemory()  const
	{
		return m_allocator.GetUsedMemory();
	}

	EOS_INLINE size GetTotalMemory() const
	{
		return m_allocator.GetTotalMemory();
	}

private:
	ActualAllocator m_allocator;
	HeaderPolicy m_header;
};

EOS_NAMESPACE_END