#pragma once

#include "Core/BasicTypes.h"
#include "Core/Assertions.h"

EOS_NAMESPACE_BEGIN

#if defined(NDEBUG)

class MemoryBoundsCheck
{
public:
	static constexpr size kSizeFront = 0;
	static constexpr size kSizeBack = 0;

	EOS_INLINE void GuardFront(void*) const {}
	EOS_INLINE void GuardBack(void*) const {}

	EOS_INLINE void CheckFront(const void*) const {}
	EOS_INLINE void CheckBack(const void*) const {}
};

#else

#define EOS_BOUND_FRONT_PATTERN 0xBEEFBABE
#define EOS_BOUND_BACK_PATTERN 0xBADDCAFE

class MemoryBoundsCheck
{
public:
	static constexpr size kSizeFront = sizeof(uint32);
	static constexpr size kSizeBack = sizeof(uint32);

	EOS_INLINE void GuardFront(void* ptr) const
	{
		*static_cast<uint32*>(ptr) = EOS_BOUND_FRONT_PATTERN;
	}

	EOS_INLINE void GuardBack(void* ptr) const
	{
		*static_cast<uint32*>(ptr) = EOS_BOUND_BACK_PATTERN;
	}

	EOS_INLINE void CheckFront(const void* ptr) const
	{
		eosAssertReturnVoid(*static_cast<const uint32*>(ptr) == EOS_BOUND_FRONT_PATTERN, "Memory bound front error");
	}

	EOS_INLINE void CheckBack(const void* ptr) const
	{
		eosAssertReturnVoid(*static_cast<const uint32*>(ptr) == EOS_BOUND_BACK_PATTERN, "Memory bound back error");
	}
};

#endif

EOS_NAMESPACE_END