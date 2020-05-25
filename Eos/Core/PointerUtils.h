#pragma once

#include "BasicDefines.h"
#include "BasicTypes.h"
#include "Assertions.h"


EOS_NAMESPACE_BEGIN


namespace CoreUtils
{
	constexpr static EOS_INLINE uintPtr AlignTop(uintPtr _ptr, size _alignment)
	{
		return ((_ptr + _alignment - 1) & ~(_alignment - 1));
	}

	constexpr static EOS_INLINE size AlignTopAmount(uintPtr _ptr, size _alignment)
	{
		return AlignTop(_ptr, _alignment) - _ptr;
	}

	constexpr static EOS_INLINE uintPtr AlignBottom(uintPtr _ptr, size _alignment)
	{
		eosAssertReturnValue(_ptr > _alignment, reinterpret_cast<uintPtr>(nullptr), "Address must greater than alignment");
		return ((_ptr) & ~(_alignment - 1));
	}

	constexpr static EOS_INLINE uintPtr AlignBottomAmount(uintPtr _ptr, size _alignment)
	{
		return _ptr - AlignBottom(_ptr, _alignment);
	}
}


EOS_NAMESPACE_END