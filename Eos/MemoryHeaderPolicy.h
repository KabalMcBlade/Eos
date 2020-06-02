#pragma once

#include "Core/NoCopyable.h"


EOS_NAMESPACE_BEGIN

/*
#if defined(NDEBUG)


class AllocationHeader : public NoCopyableMoveable
{
public:
	static constexpr size kHeaderSize = 0;

	EOS_INLINE void StoreSize(void*, size) { }
	EOS_INLINE size GetSize(void*) { return 0; }
};


#else
*/

// for realloc purpose, I keep this even in release, is not so bad in term of performance.
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


//#endif


EOS_NAMESPACE_END