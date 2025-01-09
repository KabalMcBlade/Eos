// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryTagPolicy.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BasicTypes.h"
#include "Core/Assertions.h"


EOS_NAMESPACE_BEGIN


#if defined(NDEBUG)

class MemoryTag
{
public:
	EOS_INLINE void TagAllocation(void*, size) const {}
	EOS_INLINE void TagDeallocation(void*, size) const {}
};

#else

#define EOS_TAGGING_ALLOCATED_PATTERN	0xBAADF00D
#define EOS_TAGGING_DEALLOCATED_PATTERN 0xDEADBEAF

class MemoryTag
{
public:
	EOS_INLINE void TagAllocation(void* _ptr, size _size)
	{
		TagMemory(_ptr, _size, EOS_TAGGING_ALLOCATED_PATTERN);
	}

	EOS_INLINE void TagDeallocation(void* _ptr, size _size)
	{
		TagMemory(_ptr, _size, EOS_TAGGING_DEALLOCATED_PATTERN);
	}

private:
	EOS_INLINE void TagMemory(void* _ptr, size _size, const uint32 _pattern) const
	{
		union
		{
			uint32* as_u32;
			uint8* as_byte;
		};

		uint8* start = static_cast<uint8*>(_ptr);
		uint8* current;
		for (current = start; static_cast<size>(current - start) + sizeof(uint32) <= _size; current += sizeof(uint32))
		{
			as_byte = current;
			*as_u32 = _pattern;
		}

		const uint32 numBitsToSet = static_cast<uint32>(8 * (_size - (current - start)));
		eosAssertReturnVoid(numBitsToSet <= 32, "numBitsToSet are %d, expected less or equal 32", numBitsToSet);

		// Set the remaining bytes which did not fit into a uint32 above.
		const uint32 mask = 0xFFFFFFFF >> (32 - numBitsToSet);
		as_byte = current;
		*as_u32 = (mask & _pattern) + (~mask & *as_u32);
	}
};

#endif

EOS_NAMESPACE_END