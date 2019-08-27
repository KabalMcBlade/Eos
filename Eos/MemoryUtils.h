#pragma once

#include "CoreDefs.h"

#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(_M_X64)
#include <emmintrin.h>
#endif

EOS_NAMESPACE_BEGIN

#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(_M_X64)
#include <emmintrin.h>

namespace _InternalMemory
{
	EOS_INLINE void __eosMemsetSimd(void* _ptr, eosS32 _val, eosSize _len)
	{
		eosS32 i;
		register eosS8 *p = (eosS8*)_ptr;
		for (i = 0; i < _len; ++i)
		{
			p[i] = _val;
		}
	}

	EOS_INLINE void eosMemsetSimd(void *_ptr, int _val, eosSize _len)
	{
		register eosSize i;
		eosSize loff = ((eosPtr)_ptr) % 16;
		eosSize l16 = (_len - loff) / 16;
		eosSize lrem = _len - l16 * 16 - loff;
		register eosS8 *p = (eosS8*)_ptr;
		eosS8  c = (eosS8)_val;
		__m128i c16 = _mm_set_epi8(c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
		_InternalMemory::__eosMemsetSimd(p, c, loff);
		p += loff;
		for (i = 0; i < l16; ++i)
		{
			_mm_store_si128((__m128i*)p, c16);
			p += 16;
		}
		_InternalMemory::__eosMemsetSimd(p, c, lrem);
	}

	EOS_INLINE void eosMemcpySimd(void* _dst, const void* _src, eosSize _len)
	{
		register eosU8 *dst = (eosU8*)_dst;
		register const eosU8 *src = (const eosU8*)_src;

		eosSize i = 0;
		for (; i + 128 <= _len; i += 128)
		{
			__m128i d0 = _mm_load_si128((__m128i *)&src[i + 0 * 16]);
			__m128i d1 = _mm_load_si128((__m128i *)&src[i + 1 * 16]);
			__m128i d2 = _mm_load_si128((__m128i *)&src[i + 2 * 16]);
			__m128i d3 = _mm_load_si128((__m128i *)&src[i + 3 * 16]);
			__m128i d4 = _mm_load_si128((__m128i *)&src[i + 4 * 16]);
			__m128i d5 = _mm_load_si128((__m128i *)&src[i + 5 * 16]);
			__m128i d6 = _mm_load_si128((__m128i *)&src[i + 6 * 16]);
			__m128i d7 = _mm_load_si128((__m128i *)&src[i + 7 * 16]);
			_mm_stream_si128((__m128i *)&dst[i + 0 * 16], d0);
			_mm_stream_si128((__m128i *)&dst[i + 1 * 16], d1);
			_mm_stream_si128((__m128i *)&dst[i + 2 * 16], d2);
			_mm_stream_si128((__m128i *)&dst[i + 3 * 16], d3);
			_mm_stream_si128((__m128i *)&dst[i + 4 * 16], d4);
			_mm_stream_si128((__m128i *)&dst[i + 5 * 16], d5);
			_mm_stream_si128((__m128i *)&dst[i + 6 * 16], d6);
			_mm_stream_si128((__m128i *)&dst[i + 7 * 16], d7);
		}
		for (; i + 16 <= _len; i += 16)
		{
			__m128i d = _mm_load_si128((__m128i *)&src[i]);
			_mm_stream_si128((__m128i *)&dst[i], d);
		}
		for (; i + 4 <= _len; i += 4)
		{
			*(eosU32 *)&dst[i] = *(const eosU32 *)&src[i];
		}
		for (; i < _len; ++i)
		{
			dst[i] = src[i];
		}
		_mm_sfence();
	}
}

EOS_INLINE void eosMemset(void *_ptr, int _val, eosSize _len)
{
	if (EOS_IS_ALIGNED(_ptr, 16))
	{
		_InternalMemory::eosMemsetSimd(_ptr, _val, _len);
	}
	else
	{
		memset(_ptr, _val, _len);
	}
}

EOS_INLINE void eosMemcpy(void *_ptr, const void* _src, eosSize _len)
{
	if (EOS_IS_ALIGNED(_ptr, 16) && EOS_IS_ALIGNED(_src, 16))
	{
		_InternalMemory::eosMemcpySimd(_ptr, _src, _len);
	}
	else
	{
		memcpy(_ptr, _src, _len);
	}
}

#endif

/////////////////////////////////////////////////////////////////////////////
struct eosPointerUtils
{
    constexpr static EOS_INLINE eosUPtr AlignTop(eosUPtr _ptr, eosSize _alignment)
    {
        return ((_ptr + _alignment - 1) & ~(_alignment - 1));
    }

    constexpr static EOS_INLINE eosSize AlignTopAmount(eosUPtr _ptr, eosSize _alignment)
    {
        return AlignTop(_ptr, _alignment) - _ptr;
    }

    constexpr static EOS_INLINE eosUPtr AlignBottom(eosUPtr _ptr, eosSize _alignment)
    {
        eosAssert(_ptr > _alignment, "Address must greater than alignment");
        return ((_ptr) & ~(_alignment - 1));
    }

    constexpr static EOS_INLINE eosUPtr AlignBottomAmount(eosUPtr _ptr, eosSize _alignment)
    {
        return _ptr - AlignBottom(_ptr, _alignment);
    }
};

/////////////////////////////////////////////////////////////////////////////
struct eosBitUtils
{
    constexpr static EOS_INLINE eosSize RoundUpToMultiple(eosSize _value, eosSize _multiple)
    {
        return (_value + _multiple - 1) & ~(_multiple - 1);
    }

    constexpr static EOS_INLINE eosSize RoundDownToMultiple(eosSize _value, eosSize _multiple)
    {
        eosAssert(_value > _multiple, "Value must greater than multiple");
        return (_value & ~(_multiple)) ;
    }
};

/////////////////////////////////////////////////////////////////////////////
constexpr static eosBool EOS_INLINE eosIsPowerOf2(eosSize _x)
{
    return _x && !(_x & (_x - 1));
}

/////////////////////////////////////////////////////////////////////////////
constexpr static eosU32 EOS_INLINE eosLog2(eosU32 _x)
{
	eosAssertReturnValue(_x > 0, -1, "X must be greater then 0");

	eosU32 i = 0;
	while (_x >>= 1)
	{
		++i;
	}
	return i;
}

/////////////////////////////////////////////////////////////////////////////
constexpr static eosU32 EOS_INLINE eosNextPow2(eosU32 _x)
{
	eosAssertReturnValue(_x > 0, 1, "X must be greater then 0");

	--_x;
	_x |= _x >> 1;
	_x |= _x >> 2;
	_x |= _x >> 4;
	_x |= _x >> 8;
	_x |= _x >> 16;
	++_x;

	return _x;
}

/////////////////////////////////////////////////////////////////////////////
constexpr static eosU64 EOS_INLINE eosNextPow2(eosU64 _x)
{
	eosAssertReturnValue(_x > 0, 1, "X must be greater then 0");

	--_x;
	_x |= _x >> 1;
	_x |= _x >> 2;
	_x |= _x >> 4;
	_x |= _x >> 8;
	_x |= _x >> 16;
	_x |= _x >> 32;
	++_x;

	return _x;
}

/////////////////////////////////////////////////////////////////////////////
// used to count Template instances 
template <class T>
struct eosTypeAndCount
{
};

template <class T, eosSize N>
struct eosTypeAndCount<T[N]>
{
    typedef T type;
    static const eosSize count = N;
};

/////////////////////////////////////////////////////////////////////////////
// is POD used for array allocation
template <typename T>
struct eosIsPOD
{
    static const eosBool value = std::is_pod<T>::value;
};

template <eosBool I>
struct eosIntToType
{
};

typedef eosIntToType<false> eosNonPODType;
typedef eosIntToType<true> eosPODType;


/////////////////////////////////////////////////////////////////////////////

struct eosSourceInfo
{
    eosSourceInfo(const char* _fileName, eosU32 _lineNumber) : m_fileName(_fileName), m_lineNumber(_lineNumber)
    {
    }

    const char* m_fileName;
    eosU32 m_lineNumber;
};

EOS_NAMESPACE_END


#define EOS_MEMORY_SOURCE_ALLOCATION_INFO      eos::eosSourceInfo(__FILE__, __LINE__)


