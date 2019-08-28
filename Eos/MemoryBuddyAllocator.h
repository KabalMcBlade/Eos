#pragma once

#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN


template<eosSize MinAlignment, eosU32 MaxOrder>
class eosMemoryBuddyAllocator
{
public:
	eosMemoryBuddyAllocator() :
		m_size(0)
	{
	}

	eosMemoryBuddyAllocator(void* _start, void* _end, eosSize /*_offset*/)
	{
		m_buddy.m_pool = _start;
		eosMemset(m_buddy.m_pool, 0, (reinterpret_cast<eosUPtr>(_end) - reinterpret_cast<eosUPtr>(_start)));
		m_buddy.m_freelist[kMaxOrder] = m_buddy.m_pool;
	}

	EOS_INLINE void *Alloc(eosSize _size, eosSize _alignment, eosSize _offset)
	{
		eosS32 i, order;
		void* block, *buddy;

		_size += kAllocationHeaderSize + _alignment;	// in this 32 bit more (int/Header) I'm going to store the local information I need

		// minimal order
		i = 0;
		while (GetBlockSizeOfOrder(i) < _size + _offset)
		{
			i++;
		}

		order = i = (i < kMinOrder) ? kMinOrder : i;

		// looping until a not null list
		for (;; i++) 
		{
			if (i > kMaxOrder)
			{
				return nullptr;
			}
			if (m_buddy.m_freelist[i])
			{
				break;
			}
		}

		// remove the block out of list
		block = m_buddy.m_freelist[i];

		m_buddy.m_freelist[i] = *(void**)m_buddy.m_freelist[i];

		// split until i == order
		while (i-- > order) 
		{
			buddy = GetBuddyOfOrder(block, i);

#if defined(_DEBUG)
			// The big bottleneck of this allocator using debug:
			// Because we are creating new block splitting previous allocated blocks, and because we never really allocate or delete, 
			// we need to clear this new memory from any dirty values from the previous allocations
			eosMemset(buddy, 0, GetBlockSizeOfOrder(i));
#endif

			m_buddy.m_freelist[i] = buddy;
		}

		const eosUPtr userPtr = eosPointerUtils::AlignTop(reinterpret_cast<eosUPtr>(block) + _offset, MinAlignment) - _offset;
		const eosSize offset = userPtr - reinterpret_cast<eosUPtr>(block);

		// store order in previous byte
		union
		{
			void* as_void;
			Header* as_header;
			eosUPtr as_uptr;
		};
		as_uptr = userPtr;
		Header& flag = *(as_header);
		flag = static_cast<eosU16>(offset) | (static_cast<eosU16>(order) << 16);

		++as_header;

		return as_void;
	}

	EOS_INLINE void Free(void* _ptr) 
	{
		eosS32 i;
		void* buddy;
		void** p;

		// fetch order in previous byte
		union
		{
			void* as_void;
			Header* as_header;
			eosUPtr as_uptr;
		};
		as_void = _ptr;

		--as_header;

		const Header flag = *(as_header);
		const eosU32 offset = (flag & 0x0000ffff);
		i = (flag & 0xffff0000) >> 16;

		as_uptr -= offset;

		for (;; i++) 
		{
			// calculate buddy
			buddy = GetBuddyOfOrder(as_void, i);
			p = &(m_buddy.m_freelist[i]);

			// find buddy in list
			while ((*p != nullptr) && (*p != buddy))
			{
				p = (void* *)*p;
			}

			// not found, insert into list
			if (*p != buddy) 
			{
				*(void**)as_void = m_buddy.m_freelist[i];
				m_buddy.m_freelist[i] = as_void;
				return;
			}

			// found, merged block starts from the lower one
			as_void = (as_void < buddy) ? as_void : buddy;

			// remove buddy out of list
			*p = *(void**)*p;
		}
	}


private:
	EOS_INLINE void* GetBuddyOfOrder(void* _buddy, eosS32 _order) const
	{
		eosAssertDialog(_order < 31u);

		const eosUPtr currBuddy = reinterpret_cast<eosUPtr>(_buddy);
		const eosUPtr basePool = reinterpret_cast<eosUPtr>(m_buddy.m_pool);
		const eosSize offset = currBuddy - basePool;
		const eosS32 buddyIndex = static_cast<eosS32>(offset) ^ (1 << _order);
		const eosSize buddyIndexMem = static_cast<eosSize>(buddyIndex);
		return (reinterpret_cast<void*>(buddyIndexMem + basePool));
	}

	EOS_INLINE eosU32 GetBlockSizeOfOrder(eosS32 _order) const
	{
		eosAssertDialog(_order < 31u);

		return 1 << _order;
	}

private:
	static constexpr eosU32 kMaxOrder = MaxOrder;
	static constexpr eosU32 kMinOrder = eosLog2(static_cast<eosU32>(MinAlignment));

	static_assert(kMaxOrder < 31u, "Max order for single buddy allocation is 28, this because 1 << 31 == 2 GB for single allocator");

	using Header = eosU32;
	static constexpr eosSize kAllocationHeaderSize = sizeof(Header);
	static_assert(kAllocationHeaderSize >= 4, "Header has wrong size.");

private:
	struct eosMemoryBuddy
	{
		void* m_freelist[kMaxOrder + 2];
		void* m_pool;

		eosMemoryBuddy() : m_pool(nullptr) 
		{
			eosMemset(m_freelist, 0, (kMaxOrder + 2) * sizeof(eosUPtr));
		}
	};

	eosMemoryBuddy m_buddy;
};

EOS_NAMESPACE_END