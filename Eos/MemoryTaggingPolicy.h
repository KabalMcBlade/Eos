#pragma once

#include "CoreDefs.h"

EOS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
#define EOS_TAGGING_ALLOCATED_PATTERN	0xBAADF00D
#define EOS_TAGGING_DEALLOCATED_PATTERN 0xDEADBEAF

class eosNoTagging
{
public:
    EOS_INLINE void TagAllocation(void*, eosSize) const {}
    EOS_INLINE void TagDeallocation(void*, eosSize) const {}
};

////////////////////////////////////////////////////////////////////////////////
class eosTagging
{
public:
    EOS_INLINE void TagAllocation(void* _ptr, eosSize _size)
    {
        TagMemory(_ptr, _size, EOS_TAGGING_ALLOCATED_PATTERN);
    }

    EOS_INLINE void TagDeallocation(void* _ptr, eosSize _size)
    {
        TagMemory(_ptr, _size, EOS_TAGGING_DEALLOCATED_PATTERN);
    }

private:
    EOS_INLINE void TagMemory(void* _ptr, eosSize _size, const eosU32 _pattern) const
    {
// 		eosU8 bitTags[4];
// 
// 		bitTags[0] = (_pattern & 0x000000ff);
// 		bitTags[1] = (_pattern & 0x0000ff00) >> 8;
// 		bitTags[2] = (_pattern & 0x00ff0000) >> 16;
// 		bitTags[3] = (_pattern & 0xff000000) >> 24;
// 
// 		eosU32 tagIndex = 0u;
// 		eosU8 *tagPtr = (eosU8*)_ptr;
// 		while (_size > 0)
// 		{
// 			*tagPtr = bitTags[tagIndex];
// 			tagIndex = (tagIndex + 1) % 4;
// 			--_size;
// 			++tagPtr;
// 		}

        union
        {
            eosU32* as_u32;
            eosU8* as_byte;
        };

		eosU8* start = static_cast<eosU8*>(_ptr);
		eosU8* current;
        for (current = start; static_cast<eosSize>(current - start) + sizeof(eosU32) <= _size; current += sizeof(eosU32))
        {
			as_byte = current;
            *as_u32 = _pattern;
        }

        const eosU32 numBitsToSet = static_cast<eosU32>(8 * (_size - (current - start)));
		eosAssertReturnVoid(numBitsToSet <= 32, "numBitsToSet are %d, expected less or equal 32", numBitsToSet);

        // Set the remaining bytes which did not fit into a eosU32 above.
        const eosU32 mask = 0xFFFFFFFF >> (32 - numBitsToSet);
		as_byte = current;
        *as_u32 = (mask & _pattern) + (~mask & *as_u32);
    }
};

////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
using eosDefaultTaggingPolicy = eosTagging;
#else
using eosDefaultTaggingPolicy = eosNoTagging;
#endif

EOS_NAMESPACE_END
