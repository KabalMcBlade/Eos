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
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>
#include <stdint.h>     // ionU8, etc..
#include <algorithm>    // std::min/max, etc..
#include <functional>
#include <mutex>
#include <memory>
#include <sstream>
#include <string>
#include <fstream>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include <array>
#include <random>
#include <ctime>
#include <thread>
#include <condition_variable>
#include <future>
#include <stack>
#include <queue>
#include <deque>
#include <map>



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


// default alignment
#ifdef EOS_x64
#define EOS_MEMORY_ALIGNMENT_SIZE   16
#else
#define EOS_MEMORY_ALIGNMENT_SIZE   8
#endif

// Memory alignment
#define EOS_MEMORY_ALIGNMENT(x)    __declspec(align(x))

#define EOS_IS_ALIGNED(ptr, alignment)    (((std::uintptr_t)ptr & (alignment - 1)) == 0)


// BIT MANIPULATEOS
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


#define EOS_PROFILE_START(tag) { \
const char* _tag = #tag; \
auto start = std::chrono::steady_clock::now();

#define EOS_PROFILE_END \
auto end = std::chrono::steady_clock::now(); \
auto diff = end - start; \
std::cout << "[" << _tag << "] " << "ns: " << std::chrono::duration<double, std::micro>(diff).count() << std::endl; \
}


//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

typedef std::int8_t     eosS8;
typedef std::int16_t    eosS16;
typedef std::int32_t    eosS32;
typedef std::int64_t    eosS64;

typedef std::uint8_t    eosU8;
typedef std::uint16_t   eosU16;
typedef std::uint32_t   eosU32;
typedef std::uint64_t   eosU64;

typedef std::size_t     eosSize;

typedef bool            eosBool;

typedef std::intptr_t   eosPtr;
typedef std::uintptr_t  eosUPtr;


//////////////////////////////////////////////////////////////////////////
// ASSERT
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define eosAssertVoid( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return; \
    }
#define eosAssertValue( condition, return_value, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return return_value; \
    }
#define eosAssertDialog( condition ) assert(condition)
#else
#define eosAssertVoid( condition, format, ... )
#define eosAssertValue( condition, return_value, format, ... )
#define eosAssertDialog( condition )
#endif // DEBUG

#define eosAssertReturnVoid( condition, format, ... )					eosAssertVoid( condition, format, __VA_ARGS__ )

#define eosAssertReturnValue( condition, return_value, format, ...  )	eosAssertValue( condition, return_value, format, __VA_ARGS__ )