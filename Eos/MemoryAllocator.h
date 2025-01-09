// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryAllocator.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/NoCopyable.h"
#include "MemCpy.h"

EOS_NAMESPACE_BEGIN


template <class AllocationPolicy, class ThreadPolicy, class BoundsCheckPolicy, class TagPolicy, class LogPolicy>
class MemoryAllocator : public NoCopyableMoveable
{
public:
	static_assert(BoundsCheckPolicy::kSizeBack == 0 || AllocationPolicy::kHeaderSize > 0, "BoundsCheckPolicy requires an AllocationPolicy with a header size greater than 0 in order to store allocation size.");

	static constexpr bool kAllowedAllocationArray = AllocationPolicy::kAllowedAllocationArray;

	template<typename AreaPolicy>
	MemoryAllocator(const AreaPolicy& _area, const char* _name) :
		m_headerSize(AllocationPolicy::kHeaderSize + BoundsCheckPolicy::kSizeFront),
		m_allocator(_area.GetStart(), _area.GetEnd(), m_headerSize, BoundsCheckPolicy::kSizeBack),
		m_memoryLog(_name)
#ifdef _DEBUG
		, m_debugInspectorName(_name)
#endif
	{
	}

	~MemoryAllocator()
	{
		m_memoryLog.Flush(GetAllocatedSize(), GetUsedMemory(), GetTotalMemory());
	}

	EOS_INLINE void* Allocate(size _size, size _alignment, const LogSourceInfo& _sourceInfo)
	{
		m_thread.Enter();

		const size totalSize = _size + m_headerSize + BoundsCheckPolicy::kSizeBack;

		uint8* buffer = static_cast<uint8*>(m_allocator.Allocate(totalSize, _alignment, m_headerSize, BoundsCheckPolicy::kSizeBack));

		m_allocator.StoreSize(buffer, totalSize);

		m_boundsChecker.GuardFront(buffer + AllocationPolicy::kHeaderSize);
		m_memoryTag.TagAllocation(buffer + m_headerSize, _size);
		m_boundsChecker.GuardBack(buffer + m_headerSize + _size);

		m_memoryLog.OnAllocation(buffer, totalSize, _alignment, _sourceInfo);

		m_thread.Leave();

		return (buffer + m_headerSize);
	}

	EOS_INLINE void Free(void* _ptr)
	{
		m_thread.Enter();

		uint8* buffer = static_cast<uint8*>(_ptr) - m_headerSize;
		const size totalSize = m_allocator.GetSize(buffer);
		const size allocationSize = totalSize - (m_headerSize + BoundsCheckPolicy::kSizeBack);

		m_boundsChecker.CheckBack(buffer + m_headerSize + allocationSize);
		m_memoryTag.TagDeallocation(buffer + m_headerSize, allocationSize);
		m_boundsChecker.CheckFront(buffer + AllocationPolicy::kHeaderSize);

		m_memoryLog.OnDeallocation(buffer, totalSize);

		m_allocator.Free(buffer, totalSize);

		m_thread.Leave();
	}

	EOS_INLINE void* Reallocate(void* _ptr, size _size, size _alignment, const LogSourceInfo& _sourceInfo)
	{
		if (_ptr == nullptr)
		{
			return Allocate(_size, _alignment, _sourceInfo);
		}

		m_thread.Enter();
 		uint8* buffer = static_cast<uint8*>(_ptr) - m_headerSize;
 		const size totalSize = m_allocator.GetAllocatedSize(buffer);
 		const size allocationSize = totalSize - (m_headerSize + BoundsCheckPolicy::kSizeBack);
 		const size sizeToCopy = allocationSize > _size ? _size : allocationSize;
		m_thread.Leave();

		void* newPtr = Allocate(_size, _alignment, _sourceInfo);

		m_thread.Enter();
		MemUtils::MemCpy(newPtr, _ptr, sizeToCopy);
		m_thread.Leave();

		Free(_ptr);

		return newPtr;
	}

	EOS_INLINE void Reset()
	{
		m_thread.Enter();
		m_allocator.Reset();
		m_memoryLog.Reset();
		m_thread.Leave();
	}

	EOS_INLINE size GetUsedMemory() const { return m_allocator.GetUsedMemory(); }
	EOS_INLINE size GetTotalMemory() const { return m_allocator.GetTotalMemory(); }
	EOS_INLINE size GetNumAllocations() const { return m_memoryLog.GetNumAllocations(); }
	EOS_INLINE size GetAllocatedSize() const { return m_memoryLog.GetAllocatedSize(); }

private:
	const size m_headerSize;

	AllocationPolicy m_allocator;
	BoundsCheckPolicy m_boundsChecker;
	ThreadPolicy m_thread;
	LogPolicy m_memoryLog;
	TagPolicy m_memoryTag;

#ifdef _DEBUG
	const char* m_debugInspectorName;
#endif
};

EOS_NAMESPACE_END