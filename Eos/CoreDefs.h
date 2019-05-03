#pragma once


//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <assert.h>     // for assert
#include <cstddef>      // for std::size_t
#include <cstdlib>      // for std::calloc, etc...
#include <malloc.h>     // for _aligned_malloc, malloc, etc...
#include <iostream>     // for cerr and other stuff
#include <shared_mutex> // for std::shared_mutex, std::unique_lock, etc...
#include <atomic>       // for std::atomic_uint_fast32_t, etc...

// a lot of stuff for stl
#include <typeinfo>
#include <unordered_map>
#include <atomic>
#include <utility>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <deque>
#include <queue>
#include <set>
#include <unordered_set>


//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#ifdef EOS_EXPORTS
#define EOS_DLL __declspec(dllexport)
#else
#define EOS_DLL __declspec(dllimport)
#endif 


#if _WIN32 || _WIN64
#if _WIN64
#define EOS_x64
#else
#define EOS_x86
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define EOS_x64
#else
#define EOS_x86
#endif
#else
#define EOS_x86
#endif



// defines for easy namespace-ing
#define EOS_NAMESPACE_BEGIN namespace eos {
#define EOS_NAMESPACE_END };  

#define EOS_USING_NAMESPACE using namespace eos; 

#define EOS_OPTIMIZATION_OFF __pragma(optimize("",off))
#define EOS_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define EOS_INLINE    __forceinline

// tells the compiler to never inline a particular function
#define EOS_NO_INLINE    __declspec(noinline)

// Memory alignment
#define EOS_MEMORY_ALIGNMENT(x)    __declspec(align(x))


// BIT MANIPULATION
#ifdef EOS_x64

#define EOS_BIT_SET(value, bitpos)          ((value) |= (static_cast<unsigned __int64>(1)<<(bitpos)))
#define EOS_BIT_SET_IFF(value, iff, bitpos) ((value) ^= (-iff ^ (value)) & (static_cast<unsigned __int64>(1) << (bitpos)))
#define EOS_BIT_SET_VALUE(value, mask, set) ((value) = (((value) & (mask)) | (set)))

#define EOS_BIT_CHECK(value, bitpos)        ((value) & (static_cast<unsigned __int64>(1)<<(bitpos))) 
#define EOS_BIT_CLEAR(value, bitpos)        ((value) &= ~((static_cast<unsigned __int64>(1)) << (bitpos)))
#define EOS_BIT_TOGGLE(value, bitpos)       ((value) ^= ((static_cast<unsigned __int64>(1))<<(bitpos)))

#else

#define EOS_BIT_SET(value, bitpos)          ((value) |= (1<<(bitpos)))
#define EOS_BIT_SET_IFF(value, iff, bitpos) ((value) ^= (-iff ^ (value)) & (1 << (bitpos)))
#define EOS_BIT_SET_VALUE(value, mask, set) ((value) = (((value) & (mask)) | (set)))

#define EOS_BIT_CHECK(value, bitpos)        ((value) & (1<<(bitpos))) 
#define EOS_BIT_CLEAR(value, bitpos)        ((value) &= ~((1) << (bitpos)))
#define EOS_BIT_TOGGLE(value, bitpos)       ((value) ^= (1<<(bitpos)))


#endif

#define EOS_BIT_GET(value, mask)            ((value) & (mask)) 


//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

typedef std::int8_t     eosS8;
typedef std::int32_t    eosS32;
typedef std::int64_t    eosS64;

typedef std::uint8_t    eosU8;
typedef std::uint32_t   eosU32;
typedef std::uint64_t   eosU64;

typedef std::size_t     eosSize;

typedef bool            eosBool;


//////////////////////////////////////////////////////////////////////////
// ASSERT
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define eosAssert( condition, message ) \
    if( !(condition) ) { \
        std::cerr << "Assert: " << (#condition) << std::endl; \
        std::cerr << "Message: " << message << std::endl; \
        std::cerr << "File: " << __FILE__ << std::endl; \
        std::cerr << "Line: " << __LINE__ << std::endl << std::endl; \
    }
#define eosAssertDialog( condition ) assert(condition)
#else
#define eosAssert( condition, message )
#define eosAssertDialog( condition )
#endif // DEBUG

#define eosAssertReturnVoid( condition, message ) \
    eosAssert( condition, message )\
    if( !(condition) ) { \
        return;\
    }

#define eosAssertReturnValue( condition, message, return_value ) \
    eosAssert( condition, message )\
    if( !(condition) ) { \
        return return_value;\
    }