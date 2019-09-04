#pragma once


#include "CoreDefs.h"

#include "VirtualMemory.h"
#include "MemoryUtils.h"

#include "StackAllocator.h"


EOS_NAMESPACE_BEGIN

class eosStackFromTopAllocatorPolicy
{
public:
    template<typename eosStackAllocator>
    EOS_INLINE void Init(eosStackAllocator* _stack, eosUPtr _ptr, eosSize _startSize, eosSize _maxSize, eosBool _canGrow)
    {
        _stack->m_virtualStart = _ptr + _maxSize;
        _stack->m_virtualEnd = _ptr;

        if (_canGrow)
        {
            _stack->m_physicalEnd = _stack->m_virtualStart - _startSize;
        }
        else
        {
            _stack->m_physicalEnd = _stack->m_virtualEnd;
        }
    }


    template<typename eosStackAllocator>
    EOS_INLINE void Release(eosStackAllocator* _stack)
    {
        eosVirtualMemory::ReleaseAddressSpace(reinterpret_cast<void*>(_stack->m_virtualEnd));
    }


    template<typename eosStackAllocator>
    EOS_INLINE void Reset(eosStackAllocator* _stack, eosSize _startSize, eosBool _canGrow)
    {
        _stack->m_physicalCurrent = _stack->m_virtualStart;
        _stack->m_lastPtr = _stack->m_virtualStart + eosStackAllocator::kAllocationOffset;

        if (_canGrow)
        {
            eosVirtualMemory::FreePhysicalMemory(reinterpret_cast<void*>(_stack->m_virtualEnd), (_stack->m_virtualStart - _stack->m_virtualEnd) - _startSize);
        }
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosUPtr AlignPointer(eosStackAllocator* _stack, eosSize _size, eosSize _alignment, eosSize _offset)
    {
        _offset -= eosStackAllocator::kAllocationOffset;
        return eosPointerUtils::AlignBottom(_stack->m_physicalCurrent - _size + _offset, _alignment) - _offset - eosStackAllocator::kAllocationOffset;
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosU32 CalculateHeaderSize(eosStackAllocator* _stack, eosUPtr _prevCurrentPtr, eosSize _size)
    {
        return static_cast<eosU32>(_prevCurrentPtr - (_stack->m_physicalCurrent + _size));
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosBool IsOutOfMemory(eosStackAllocator* _stack, eosSize /*_size*/)
    {
        return _stack->m_physicalCurrent < _stack->m_physicalEnd;
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosBool Grow(eosStackAllocator* _stack, eosSize _allocationSize, eosBool _hugeMemoryPage)
    {
        const eosSize neededPhysicalSize = eosBitUtils::RoundUpToMultiple(_allocationSize, _stack->m_growSize);
        if (_stack->m_physicalEnd - neededPhysicalSize < _stack->m_virtualEnd)
        {
            return false;
        }

        eosVirtualMemory::CommitPhysicalMemory(reinterpret_cast<void*>(_stack->m_physicalEnd - neededPhysicalSize), neededPhysicalSize/*, _hugeMemoryPage*/);
        _stack->m_physicalEnd -= neededPhysicalSize;

        return true;
    }

    template<typename eosStackAllocator>
    EOS_INLINE void* Allocate(eosStackAllocator* _stack, eosU32 _headerSize, eosSize /*_size*/)
    {
        union
        {
            void* as_void;
            eosU8* as_char;
            eosU32* as_u32;
            eosUPtr as_uptr;
        };

        as_uptr = _stack->m_physicalCurrent;

        const eosUPtr lastPtrOffset = _stack->m_lastPtr - as_uptr;
        *as_u32 = static_cast<eosU32>(lastPtrOffset);
        *(as_u32 + 1) = _headerSize;
        as_char += eosStackAllocator::kAllocationOffset;

        void* userPtr = as_void;
        _stack->m_lastPtr = as_uptr;
        _stack->m_physicalCurrent = _stack->m_lastPtr - eosStackAllocator::kAllocationOffset;

        return userPtr;
    }

    template<typename eosStackAllocator>
    EOS_INLINE void Free(eosStackAllocator* _stack, void* _ptr)
    {
        union
        {
            void* as_void;
            eosU8* as_char;
            eosU32* as_u32;
            eosUPtr as_uptr;
        };

        as_void = _ptr;

        as_char -= eosStackAllocator::kAllocationOffset;
        const eosU32 lastPtrOffset = *as_u32;
        const eosU32 headerSize = *(as_u32 + 1);
        
        eosAssertDialog(headerSize >= eosStackAllocator::kAllocationOffset);

        _stack->m_lastPtr = as_uptr + lastPtrOffset;
        _stack->m_physicalCurrent = _stack->m_lastPtr - eosStackAllocator::kAllocationOffset;
    }

    template<typename eosStackAllocator>
    EOS_INLINE void Purge(eosStackAllocator* _stack)
    {
        eosUPtr addressToFree = eosPointerUtils::AlignBottom(_stack->m_physicalCurrent, _stack->m_growSize);

        if (addressToFree >= _stack->m_physicalEnd)
        {
            const eosSize sizeToFree = addressToFree - _stack->m_physicalEnd;
            if (sizeToFree > 0)
            {
                eosVirtualMemory::FreePhysicalMemory(reinterpret_cast<void*>(_stack->m_physicalEnd), sizeToFree);
                _stack->m_physicalEnd = addressToFree;
            }
        }
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosSize GetTotalUsedSize(eosStackAllocator* _stack) const
    {
        return _stack->m_virtualStart - _stack->m_physicalCurrent;
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosSize GetVirtualSize(eosStackAllocator* _stack) const
    {
        return _stack->m_virtualStart - _stack->m_virtualEnd;
    }

    template<typename eosStackAllocator>
    EOS_INLINE eosSize GetPhysicalSize(eosStackAllocator* _stack) const
    {
        return _stack->m_virtualStart - _stack->m_physicalEnd;
    }
};



EOS_NAMESPACE_END