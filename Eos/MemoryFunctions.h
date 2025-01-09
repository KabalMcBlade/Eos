// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryFunctions.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BasicTypes.h"

#include "MemoryLayoutUtils.h"
#include "MemoryLogPolicy.h"


EOS_NAMESPACE_BEGIN


template<typename T, class Allocator>
EOS_INLINE void Free(T* _object, Allocator* _allocator)
{
	_object->~T();
	_allocator->Free(_object);
}


template<typename T, class Allocator>
EOS_INLINE T* AllocArray(Allocator* _allocator, size _cnt, const char* _file, uint32 _line, MemUtils::NoPODType)
{
	eosAssertReturnValue(Allocator::kAllowedAllocationArray, nullptr, "This allocator cannot allocates array using this proxy function, please check the allocator for further details!");

	union
	{
		void* as_void;
		size* as_size;
		T* as_T;
	};

	as_T = AllocArray<T>(_allocator, _cnt, _file, _line, MemUtils::PODType());

	const T* const onePastLast = as_T + _cnt;
	while (as_T < onePastLast)
	{
		new (as_T++) T;
	}

	return as_T - _cnt;
}

template<typename T, class Allocator>
EOS_INLINE T* AllocArray(Allocator* _allocator, size _cnt, const char* _file, uint32 _line, MemUtils::PODType)
{
	eosAssertReturnValue(Allocator::kAllowedAllocationArray, nullptr, "This allocator cannot allocates array using this proxy function, please check the allocator for further details!");

	union
	{
		void* as_void;
		size* as_size;
		T* as_T;
	};

	const size totalSize = (sizeof(T) * _cnt) + sizeof(size);	// the size of size is to store the element in front of the pointer
	as_void = _allocator->Allocate(totalSize, alignof(T), LogSourceInfo(_file, _line));

	*(as_size) = _cnt;
	++as_size;	// increment of size, to skip the first part where the count is stored

	return as_T;
}


template <typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator* _allocator)
{
	eosAssertReturnVoid(Allocator::kAllowedAllocationArray, "This allocator cannot allocates array using this proxy function, please check the allocator for further details!");

	FreeArray(_ptr, _allocator, MemUtils::IntToType<MemUtils::IsPOD<T>::value>());
}


template<typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator* _allocator, MemUtils::NoPODType)
{
	eosAssertReturnVoid(Allocator::kAllowedAllocationArray, "This allocator cannot allocates array using this proxy function, please check the allocator for further details!");

	union
	{
		size* as_size;
		T* as_T;
	};

	as_T = _ptr;

	const size _cnt = as_size[-1];	// this should be valid, since I have allocated one more before and returned the ++size beforehand during allocation

	for (size i = _cnt; i > 0; --i)
	{
		as_T[i - 1].~T();
	}

	--as_size;
	_allocator->Free(as_T);
}


template<typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator& _allocator, MemUtils::PODType)
{
	eosAssertReturnVoid(Allocator::kAllowedAllocationArray, "This allocator cannot allocates array using this proxy function, please check the allocator for further details!");

	union
	{
		size* as_size;
		T* as_T;
	};

	as_T = _ptr;
	--as_size;
	_allocator->Free(as_T);
}


EOS_NAMESPACE_END


#define eosNewAlignedRaw(Size, Allocator, Alignment)	 (Allocator)->Allocate(Size, Alignment, EOS_ALLOCATION_INFO)
#define eosDeleteRaw(Ptr, Allocator)					 (Allocator)->Free(Ptr);

#define eosNewAligned(Type, Allocator, Alignment, ...)  new ((Allocator)->Allocate(sizeof(Type), Alignment, EOS_ALLOCATION_INFO)) Type(__VA_ARGS__)
#define eosNew(Type, Allocator, ...)                    eosNewAligned(Type, (Allocator), alignof(Type), __VA_ARGS__)
#define eosDelete(Object, Allocator)                    eos::Free((Object), (Allocator))

#define eosReallocAligned(Ptr, Type, Allocator, Alignment)		(Allocator)->Reallocate(Ptr, sizeof(Type), Alignment, EOS_ALLOCATION_INFO)
#define eosReallocAlignedRaw(Ptr, Size, Allocator, Alignment)	(Allocator)->Reallocate(Ptr, Size, Alignment, EOS_ALLOCATION_INFO)

#define eosNewDynamicArray(Type, Count, Allocator)      eos::AllocArray<Type>((Allocator), Count, __FILE__, __LINE__, eos::MemUtils::IntToType<eos::MemUtils::IsPOD<Type>::value>())
#define eosNewArray(Type, Allocator)                    eos::AllocArray<eos::MemUtils::TypeAndCount<Type>::type>((Allocator), eos::MemUtils::TypeAndCount<Type>::count, __FILE__, __LINE__, eos::MemUtils::IntToType<eos::MemUtils::IsPOD<eos::MemUtils::TypeAndCount<Type>::type>::value>())
#define eosDeleteArray(ObjectArray, Allocator)          eos::FreeArray((ObjectArray), (Allocator))