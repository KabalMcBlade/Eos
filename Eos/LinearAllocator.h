#pragma once

#include "CoreDefs.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class LinearAllocator
{
public:

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

    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            eosAssertReturnValue(m_uipLinear, "Linear allocator is not allocated", nullptr);
            eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be less or equal to the allocator alignment (16)", nullptr);

            eosSize uiMask = _uiAlignment - 1;
            eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

            eosU8* uipCurrentLinear = m_uipLast;
            eosU8* uipResult = uipCurrentLinear;

            uipCurrentLinear += uiSize;

            eosAssertReturnValue(uipCurrentLinear >= m_uipLast && uipCurrentLinear < m_uipLinear + m_uiSize * sizeof(eosU8), "Pointer is out of memory", nullptr);

            m_uipLast = uipCurrentLinear;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
            m_log.WriteAlloc(_uiSize, _uiAlignment, uiSize, uiSize, uipResult);
#endif

            return uipResult;
        }
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
        // I know is stupid, but just for coherence
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be less or equal to the allocator alignment", nullptr);

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteCustomMessage("[NO REALLOC AVAIALBE]: DON'T CHECK MEMORY IN THIS CONTEXT!");
#endif

        // Realloc is not allowed
        return _uipBuffer;
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

EOS_NAMESPACE_END
