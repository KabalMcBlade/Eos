// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryHeaderPolicy.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/NoCopyable.h"


EOS_NAMESPACE_BEGIN


#if defined(NDEBUG)

class AllocationHeader : public NoCopyableMoveable
{
public:
	static constexpr size kHeaderSize = 0;

	EOS_INLINE void StoreSize(void*, size) { }
	EOS_INLINE size GetSize(void*) { return 0; }
};


#else

class AllocationHeader : public NoCopyableMoveable
{
public:
	static constexpr size kHeaderSize = sizeof(uint32);

	EOS_INLINE void StoreSize(void* _ptr, size _size)
	{
		*((uint32*)_ptr) = static_cast<uint32>(_size);
	}

	EOS_INLINE size GetSize(void* _ptr)
	{
		return *((uint32*)_ptr);
	}
};

#endif


EOS_NAMESPACE_END