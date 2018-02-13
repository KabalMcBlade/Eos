#pragma once


//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <assert.h>     // for assert
#include <cstddef>      // for std::size_t
#include <cstdlib>      // for std::calloc, etc...
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


// comment to avoid memory trace
// Anyway this work ONLY in debug due the _DEBUG defined in the project options
#define EOS_MEMORYLOAD


// defines for easy namespace-ing
#define EOS_NAMESPACE_BEGIN namespace eos {
#define EOS_NAMESPACE_END };  

#define EOS_USING_NAMESPACE using namespace eos; 

#define EOS_MEMORY_ALIGNMENT_SIZE 16


#define EOS_OPTIMIZATION_OFF __pragma(optimize("",off))
#define EOS_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define EOS_INLINE    __forceinline

// tells the compiler to never inline a particular function
#define EOS_NO_INLINE    __declspec(noinline)

// Memory alignment
#define EOS_MEMORY_ALIGNMENT(x)    __declspec(align(x))
#define EOS_IS_ALIGNED(ptr, alignment)    ((uint_fast64_t)ptr & (alignment - 1)) == 0

// BIT MANIPULATION
#define EOS_BIT_SET(value, bitpos)          ((value) |= (1<<(bitpos)))
#define EOS_BIT_SET_IFF(value, iff, bitpos) ((value) ^= (-iff ^ (value)) & (1 << (bitpos)))
#define EOS_BIT_SET_VALUE(value, mask, set) ((value) = (((value) & (mask)) | (set)))

#define EOS_BIT_CHECK(value, bitpos)        ((value) & (1<<(bitpos))) 
#define EOS_BIT_CLEAR(value, bitpos)        ((value) &= ~((1) << (bitpos)))
#define EOS_BIT_TOGGLE(value, bitpos)       ((value) ^= (1<<(bitpos)))
#define EOS_BIT_GET(value, mask)	        ((value) & (mask)) 


//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

typedef	bool			eosBool;
typedef uint_fast8_t    eosU8;
typedef uint_fast32_t   eosU32;
typedef uint_fast64_t   eosU64;
typedef int_fast8_t     eosS8;
typedef int_fast32_t    eosS32;
typedef int_fast64_t    eosS64;
typedef std::size_t     eosSize;


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