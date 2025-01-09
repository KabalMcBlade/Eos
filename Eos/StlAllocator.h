// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\StlAllocator.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BasicTypes.h"

#include "MemoryAllocator.h"
#include "MemoryLogPolicy.h"


EOS_NAMESPACE_BEGIN


template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align>
class StlAllocator;


// specialize for void:
template <typename Allocator, Allocator*(*_AllocatorCallback)(void)>
class StlAllocator<void, Allocator, _AllocatorCallback, 0>
{
public:	
	static_assert(Allocator::kAllowedAllocationArray, "StlAllocator container has to allow the allocation of an array");

	typedef void*       pointer;
	typedef const void* const_pointer;

	// reference to void members are impossible
	typedef void        value_type;

	// A struct to rebind the allocator to another allocator of type U
	template <typename U>
	struct rebind
	{
		typedef StlAllocator<U, Allocator, _AllocatorCallback, 0> other;
	};
};


template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)>
class StlAllocator
{
private:
	template<typename U, typename AllocatorU, AllocatorU*(*_AllocatorCallbackU)(void), size AlignU = __alignof(T)>
	friend class StlAllocator;

public:
	// A struct to rebind the allocator to another allocator of type U
	template<typename U>
	struct rebind
	{
		typedef StlAllocator<U, Allocator, _AllocatorCallback, Align> other;
	};

public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	static_assert(Allocator::kAllowedAllocationArray, "StlAllocator container has to allow the allocation of an array");

	StlAllocator()
	{
		m_allocator = _AllocatorCallback();
	}

	~StlAllocator()
	{
	}

	StlAllocator(StlAllocator const& _other) noexcept
	{
		m_allocator = _other.m_allocator;
	}

	template<typename U>
	StlAllocator(StlAllocator<U, Allocator, _AllocatorCallback, Align> const& _other) noexcept
	{
		m_allocator = _other.m_allocator;
	}

	StlAllocator& operator=(const StlAllocator& _other)
	{
		m_allocator = _other.m_allocator;
		return *this;
	}

	template<typename U>
	StlAllocator& operator=(const StlAllocator<U, Allocator, _AllocatorCallback, Align>& _other)
	{
		m_allocator = _other.m_allocator;
		return *this;
	}

	// Get the address of a reference
	pointer address(reference _address) const
	{
		return &_address;
	}

	// Get the const address of a const reference
	const_pointer address(const_reference _address) const
	{
		return &_address;
	}

	// Allocate memory
	pointer allocate(size_type cnt, const void* hint = 0)
	{
		eosAssert(m_allocator, "Allocator is null!");

		const size_type size = cnt * sizeof(value_type);

		void* pAllocation = m_allocator->Allocate(size, Align, EOS_ALLOCATION_INFO);

		eosAssertReturnValue(pAllocation != nullptr, nullptr, "StlAllocator failed to allocate memory.");

		return (pointer)pAllocation;
	}

	// Deallocate memory
	void deallocate(void* p, size_type cnt)
	{
		eosAssert(m_allocator, "Allocator is null!");

		m_allocator->Free(p);
	}

	// Call constructor
	void construct(pointer p, const_reference val)
	{
		// Placement new
		new ((pointer)p) T(val);
	}
	
	// Call constructor
	template <typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
	}
	
	// Call destructor
	void destroy(pointer p)
	{
		p->~T();
	}

	// Call destructor
	template<typename U>
	void destroy(U* p)
	{
		p->~U();
	}

	// Get the max allocation size
	size_type max_size() const
	{
#ifdef max
#undef max
#endif
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	void Reset()
	{
		eosAssert(m_allocator, "Allocator is null!");

		m_allocator->Reset();
	}

protected:
	Allocator* m_allocator;
};



// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, typename Allocator1, typename Allocator2, Allocator1*(*_AllocatorCallback1)(void), Allocator2*(*_AllocatorCallback2)(void), size Align1 = _alignof(T1), size Align2 = _alignof(T2)>
inline bool operator==(const StlAllocator<T1, Allocator1, _AllocatorCallback1, Align1>& a, const StlAllocator<T2, Allocator2, _AllocatorCallback2, Align2>& b)
{
	return _AllocatorCallback1 == _AllocatorCallback2;
}

// Another allocator of the another type cannot deallocate from this one
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = _alignof(T), typename Other>
inline bool operator==(const StlAllocator<T, Allocator, _AllocatorCallback, Align>&, const Other&)
{
	return false;
}

// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, typename Allocator1, typename Allocator2, Allocator1*(*_AllocatorCallback1)(void), Allocator2*(*_AllocatorCallback2)(void), size Align1 = _alignof(T1), size Align2 = _alignof(T2)>
inline bool operator!=(const StlAllocator<T1, Allocator1, _AllocatorCallback1, Align1>& a, const StlAllocator<T2, Allocator2, _AllocatorCallback2, Align2>& b)
{
	return _AllocatorCallback1 != _AllocatorCallback2;
}

// Another allocator of the another type cannot deallocate from this one
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = _alignof(T), typename Other>
inline bool operator!=(const StlAllocator<T, Allocator, _AllocatorCallback, Align>&, const Other&)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////
//                  HASH
///////////////////////////////////////////////////////////////////////////

template<typename T, typename U>
class hash_pair
{
public:
	size operator()(const std::pair<T, U>& _hash) const
	{
		size a = static_cast<size>(std::hash<T>()(_hash.first));
		size b = static_cast<size>(std::hash<U>()(_hash.second));
		return a ^ b;
	}
};


EOS_NAMESPACE_END