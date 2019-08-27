#pragma once

#include "CoreDefs.h"

EOS_NAMESPACE_BEGIN


class eosNoAllocationHeader
{
public:
    static constexpr eosSize kHeaderSize = 0;

    EOS_INLINE void StoreAllocationSize(void*, eosSize) { }
    EOS_INLINE eosSize GetAllocationSize(void*) { return 0; }
};

////////////////////////////////////////////////////////////////////////////////
template<typename Type>
class eosAllocationHeader
{
public:
    static constexpr eosSize kHeaderSize = sizeof(Type);

    EOS_INLINE void StoreAllocationSize(void* _ptr, eosSize _size)
    {
        *((Type*)_ptr) = static_cast<Type>(_size);
    }

    EOS_INLINE eosSize GetAllocationSize(void* _ptr)
    {
        return *((Type*)_ptr);
    }
};



#if defined(_DEBUG)
using eosAllocationHeaderU8 = eosAllocationHeader<eosU8>;
using eosAllocationHeaderU16 = eosAllocationHeader<eosU16>;
using eosAllocationHeaderU32 = eosAllocationHeader<eosU32>;
using eosAllocationHeaderU64 = eosAllocationHeader<eosU64>;
#else
using eosAllocationHeaderU8 = eosNoAllocationHeader;
using eosAllocationHeaderU16 = eosNoAllocationHeader;
using eosAllocationHeaderU32 = eosNoAllocationHeader;
using eosAllocationHeaderU64 = eosNoAllocationHeader;
#endif


EOS_NAMESPACE_END
