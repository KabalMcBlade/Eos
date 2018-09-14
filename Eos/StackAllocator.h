#pragma once

#include "CoreDefs.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class StackAllocator
{
public:
    EOS_INLINE void Init(eosSize _uiBlockSize, eosSize _uiBlockCount)
    {
        eosAssertReturnVoid(!(_uiBlockSize % 2) && !(_uiBlockCount % 2), "Stack allocator: Block count and Block size must be multiple of 2!");

        m_uiBlockSize = _uiBlockSize;
        m_uiBlockCount = _uiBlockCount;

#ifdef EOS_x64
        m_uipBlocks = (eosSize*)calloc(m_uiBlockCount, sizeof(eosSize));
#else
        m_uipBlocks = (eosSize*)_aligned_malloc(m_uiBlockCount * sizeof(eosSize), EOS_MEMORY_ALIGNMENT_SIZE);
        memset(m_uipBlocks, 0, m_uiBlockCount * sizeof(eosSize));
#endif // EOS_x64

        eosAssert(m_uipBlocks, "Memory is not allocated!");

        eosSize uiMask = EOS_MEMORY_ALIGNMENT_SIZE - 1;
        eosSize totalSize = ((m_uiBlockSize * m_uiBlockCount) + uiMask) & ~uiMask;

#ifdef EOS_x64
        m_uipStack = (eosU8*)calloc(totalSize, sizeof(eosU8));
#else
        m_uipStack = (eosU8*)_aligned_malloc(totalSize * sizeof(eosU8), EOS_MEMORY_ALIGNMENT_SIZE);
        memset(m_uipStack, 0, totalSize * sizeof(eosU8));
#endif // EOS_x64

        eosAssert(m_uipStack, "Memory is not allocated!");

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Init("stack.log", totalSize);
#endif
    }

    EOS_INLINE void Shutdown()
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Shutdown();
#endif

#ifdef EOS_x64
        free(m_uipStack);
        free(m_uipBlocks);
#else
        _aligned_free(m_uipStack);
        _aligned_free(m_uipBlocks);
#endif // EOS_x64

        m_uipStack = nullptr;
        m_uipBlocks = nullptr;
        m_uiBlockCount = 0;
        m_uiBlockSize = 0;
    }

    // in this version, the first time the allocation is called, set the size of the block
    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            eosAssertReturnValue(m_uipStack, "Stack allocator is not allocated", nullptr);
            eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be less or equal to the allocator alignment", nullptr);

            eosSize uiMask = _uiAlignment - 1;
            eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

            eosAssertReturnValue(uiSize < m_uiBlockSize, "Pointer is out of memory", nullptr);

            // find a free block
            eosBool bFoundBlock = false;
            eosSize uiBlock = 0;
            for (eosSize i = 0; i < m_uiBlockCount; ++i)
            {
                // there are a free block?
                if (m_uipBlocks[i] == 0)
                {
                    m_uipBlocks[i] = 1;
                    uiBlock = i;
                    bFoundBlock = true;
                    break;
                }
            }

            eosAssertReturnValue(bFoundBlock && uiBlock < m_uiBlockCount, "There are any free block in the stack", nullptr);

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
            m_log.WriteAlloc(_uiSize, _uiAlignment, uiSize, (uiBlock * m_uiBlockSize), m_uipStack + (uiBlock * m_uiBlockSize));
#endif
            // return memory block
            return m_uipStack + (uiBlock * m_uiBlockSize);
        }
    }

    EOS_INLINE void Free(void *_uipBuffer)
    {
        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            eosU8 *uipBuffer = static_cast<eosU8 *>(_uipBuffer);

            // check if this pointer is inside our list
            eosAssertReturnVoid(uipBuffer >= m_uipStack && uipBuffer < m_uipStack + m_uiBlockSize * m_uiBlockCount * sizeof(eosU8), "Pointer is not in the stack");

            // convert block
            eosSize uiBlock = static_cast<eosSize>(uipBuffer - m_uipStack) / m_uiBlockSize;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
            m_log.WriteFree((uiBlock * m_uiBlockSize), _uipBuffer);
#endif

            m_uipBlocks[uiBlock] = 0;
        }
    }


    // TODO: Check if it works.... in the stack couldn't work
    EOS_INLINE void* Reallocate(void *_uipBuffer, eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be less or equal to the allocator alignment", nullptr);

        void* ptr = Alloc(_uiSize, _uiAlignment);

        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            memcpy(ptr, _uipBuffer, _uiSize);
        }

        Free(_uipBuffer);

        return ptr;  
    }

private:
    eosSize m_uiBlockSize;
    eosSize m_uiBlockCount;
    eosSize* m_uipBlocks;
    eosU8* m_uipStack;

    typedef std::shared_mutex               SharedMutex;
    typedef std::unique_lock<SharedMutex>   SharedMutexUniqueLock;

    SharedMutex m_memoryMutex;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
    Log m_log;
#endif
};

EOS_NAMESPACE_END
