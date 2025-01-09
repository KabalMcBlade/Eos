// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryLayoutUtils.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BasicDefines.h"
#include "Core/BasicTypes.h"

#include "MemoryBasicDefines.h"

EOS_NAMESPACE_BEGIN

namespace MemUtils
{
	// used to count Template instances 
	template <class T>
	struct TypeAndCount
	{
	};

	template <class T, size N>
	struct TypeAndCount<T[N]>
	{
		typedef T type;
		static const size count = N;
	};


	// is POD used for array allocation
	template <typename T>
	struct IsPOD
	{
		static const bool value = std::is_pod<T>::value;
	};

	template <bool I>
	struct IntToType
	{
	};

	typedef IntToType<false> NoPODType;
	typedef IntToType<true> PODType;

}

EOS_NAMESPACE_END