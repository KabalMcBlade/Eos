#pragma once

#include "CoreDefs.h"
#include "MemoryUtils.h"

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>

EOS_NAMESPACE_BEGIN

class eosVirtualMemory
{
public:
    static EOS_INLINE eosSize GetPageSize()
    {
        static eosSize pageSize = 0;
        if (pageSize == 0)
        {
            SYSTEM_INFO systemInfo;
			eosMemset(&systemInfo, 0, sizeof(systemInfo));
            GetSystemInfo(&systemInfo);

            pageSize = systemInfo.dwPageSize;
        }
        return pageSize;
    }

    static EOS_INLINE eosSize GetPageGranularity()
    {
        static eosSize pageGranularity = 0;
        if (pageGranularity == 0)
        {
            SYSTEM_INFO systemInfo;
			eosMemset(&systemInfo, 0, sizeof(systemInfo));
            GetSystemInfo(&systemInfo);

            pageGranularity = systemInfo.dwAllocationGranularity;
        }
        return pageGranularity;
    }

    static EOS_INLINE eosSize GetPagePadding(eosSize _size)
    {
        return ((_size >= kMemorySpanSize) && (kMemorySpanSize > GetPageGranularity())) ? kMemorySpanSize : 0;
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    static EOS_INLINE void* ReserveAddressSpace(eosSize _size)
    {
        void* ptr = VirtualAlloc(0, _size + GetPagePadding(_size), MEM_RESERVE, PAGE_NOACCESS);

        eosAssertReturnValue(ptr, nullptr, "Failed to reserve virtual memory.");

        return ptr;
    }

    static EOS_INLINE void* CommitPhysicalMemory(void* _ptr, eosSize _size)
    {
        void* ptr = VirtualAlloc(_ptr, _size + GetPagePadding(_size), MEM_COMMIT, PAGE_READWRITE);

        eosAssertReturnValue(ptr, nullptr, "Failed to commit virtual memory.");

        return ptr;
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    static EOS_INLINE void* AllocatePhysicalMemory(eosSize _size)
    {
        void* ptr = VirtualAlloc(0, _size + GetPagePadding(_size), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        eosAssertReturnValue(ptr, nullptr, "Failed to map virtual memory.");

        return ptr;
    }

    static EOS_INLINE void* AllocatePhysicalMemory(void* _ptr, eosSize _size)
    {
        void* ptr = VirtualAlloc(_ptr, _size + GetPagePadding(_size), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        eosAssertReturnValue(ptr, nullptr, "Failed to map virtual memory.");

        return ptr;
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    static EOS_INLINE void FreePhysicalMemory(void* _ptr, eosSize  _size)
    {
        const eosBool result = VirtualFree(_ptr, _size, MEM_DECOMMIT);

        eosAssertReturnVoid(result, "Failed to decommit virtual memory.");
    }

    static EOS_INLINE void ReleaseAddressSpace(void* _ptr)
    {
        const eosBool result = VirtualFree(_ptr, 0, MEM_RELEASE);

        eosAssertReturnVoid(result, "Failed to release virtual memory.");
    }

private:
    static constexpr eosSize kMemorySpanSize = 65536;
};


EOS_NAMESPACE_END