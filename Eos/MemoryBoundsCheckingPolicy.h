#pragma once

#include "CoreDefs.h"

EOS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
#define EOS_BOUND_FRONT_PATTERN 0xCAFECAFE
#define EOS_BOUND_BACK_PATTERN 0xBADDCAFE

class eosNoBoundsChecking
{
public:
    static constexpr eosSize kSizeFront = 0;
    static constexpr eosSize kSizeBack = 0;

    EOS_INLINE void GuardFront(void*) const {}
    EOS_INLINE void GuardBack(void*) const {}

    EOS_INLINE void CheckFront(const void*) const {}
    EOS_INLINE void CheckBack(const void*) const {}
};

////////////////////////////////////////////////////////////////////////////////
class eosBoundsChecking
{
public:
    static constexpr eosSize kSizeFront = sizeof(eosU32);
    static constexpr eosSize kSizeBack = sizeof(eosU32);

    EOS_INLINE void GuardFront(void* ptr) const
    {
        *static_cast<eosU32*>(ptr) = EOS_BOUND_FRONT_PATTERN;
    }

    EOS_INLINE void GuardBack(void* ptr) const
    {
        *static_cast<eosU32*>(ptr) = EOS_BOUND_BACK_PATTERN;
    }

    EOS_INLINE void CheckFront(const void* ptr) const
    {
        eosAssertReturnVoid(*static_cast<const eosU32*>(ptr) == EOS_BOUND_FRONT_PATTERN, "Memory bound front error");
    }

    EOS_INLINE void CheckBack(const void* ptr) const
    {
		eosAssertReturnVoid(*static_cast<const eosU32*>(ptr) == EOS_BOUND_BACK_PATTERN, "Memory bound back error");
    }
};

////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
using eosDefaultBoundsCheckingPolicy = eosBoundsChecking;
#else
using eosDefaultBoundsCheckingPolicy = eosNoBoundsChecking;
#endif

EOS_NAMESPACE_END
