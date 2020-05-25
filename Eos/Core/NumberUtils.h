#pragma once

#include "BasicDefines.h"
#include "BasicTypes.h"
#include "Assertions.h"


EOS_NAMESPACE_BEGIN


namespace CoreUtils
{
	template<typename T>
	constexpr static bool EOS_INLINE IsPowerOf2(T _x)
	{
		return _x && !(_x & (_x - 1));
	}

	template<typename T>
	constexpr static T EOS_INLINE Log2(T _x)
	{
		eosAssertReturnValue(_x > 0, -1, "X must be greater then 0");

		T i = 0;
		while (_x >>= 1)
		{
			++i;
		}
		return i;
	}

	template<typename T>
	constexpr static T EOS_INLINE NextPow2(T _x)
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
}


EOS_NAMESPACE_END