#pragma once

#include "CoreDefs.h"
#include "MemoryDefines.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class LinearAllocator
{
public:
    EOS_INLINE LinearAllocator()
    {
        Init(EOS_LINEAR_MEMORY);
    }

    EOS_INLINE ~LinearAllocator()
    {
        Shutdown();
    }

    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(_uiSize > 0, "Size must be passed greater then 0", nullptr);
        eosAssertReturnValue(IsPowerOf2(_uiAlignment), "Alignment must be power of 2", nullptr);
        eosAssertReturnValue(m_uipLinear, "Heap allocator is not allocated", nullptr);

        SharedMutexUniqueLock lock(m_memoryMutex);

        const eosSize uiMask = _uiAlignment - 1;
        const eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

        eosU8* uipCurrentLinear = m_uipLast;
        eosU8* uipResult = uipCurrentLinear;

        uipCurrentLinear += uiSize;

        eosAssertReturnValue(uipCurrentLinear >= m_uipLast && uipCurrentLinear < m_uipLinear + m_uiSize, "Pointer is out of memory", nullptr);

        m_uipLast = uipCurrentLinear;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteAlloc(_uiSize, _uiAlignment, uiSize, uiSize, uipResult);
#endif

        return uipResult;
    }

    EOS_INLINE void Free(void *_uipBuffer)
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteCustomMessage("[NO FREE AVAIALBE]: DON'T CHECK MEMORY IN THIS CONTEXT!");
#endif
        // Free is not allowed
    }

    EOS_INLINE void* Reallocate(void *_uipBuffer, eosSize _uiSize, eosSize _uiAlignment)
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteCustomMessage("[NO REALLOC AVAIALBE]: DON'T CHECK MEMORY IN THIS CONTEXT!");
#endif

        // Realloc is not allowed
        return _uipBuffer;
    }

    EOS_INLINE eosBool IsPointerOwned(void *_uipBuffer)
    {
        return (_uipBuffer >= m_uipLinear && _uipBuffer <= m_uipLinear + m_uiSize);
    }

private:
    EOS_INLINE eosBool IsPowerOf2(eosSize _x)
    {
        return _x && !(_x & (_x - 1));
    }

    EOS_INLINE void Init(eosSize _uiSize)
    {
        eosSize uiMask = EOS_MEMORY_ALIGNMENT_SIZE - 1;
        m_uiSize = (_uiSize + uiMask) & ~uiMask;

#ifdef EOS_x64
        m_uipLinear = (eosU8*)calloc(m_uiSize, sizeof(eosU8));
#else
        m_uipLinear = (eosU8*)_aligned_malloc(m_uiSize * sizeof(eosU8), EOS_MEMORY_ALIGNMENT_SIZE);
        memset(m_uipLinear, 0, m_uiSize * sizeof(eosU8));
#endif // EOS_x64

        eosAssert(m_uipLinear, "Memory is not allocated!");

        m_uipLast = m_uipLinear;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Init("linear.log", m_uiSize);
#endif
    }

    EOS_INLINE void Shutdown()
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Shutdown();
#endif

#ifdef EOS_x64
        free(m_uipLinear);
#else
        _aligned_free(m_uipLinear);
#endif // EOS_x64

        m_uipLinear = nullptr;
        m_uipLast = nullptr;
        m_uiSize = 0;
    }

private:
    eosU8* m_uipLinear;
    eosU8* m_uipLast;
    eosSize m_uiSize;

    typedef std::shared_mutex               SharedMutex;
    typedef std::unique_lock<SharedMutex>   SharedMutexUniqueLock;

    SharedMutex m_memoryMutex;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
    Log m_log;
#endif
};

extern "C" EOS_DLL LinearAllocator g_linearAllocator;

EOS_NAMESPACE_END
