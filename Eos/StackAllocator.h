#pragma once

#include "CoreDefs.h"
#include "MemoryDefines.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

#ifdef EOS_x64
#define EOS_STACK_HEADER_BIT_HAS_BEEN_ALLOCATED     63
#define EOS_STACK_HEADER_MASK_COUNT_READ            0x7FFFFFFFFFFFFFFF
#define EOS_STACK_HEADER_MASK_COUNT_WRITE           0x8000000000000000
#else
#define EOS_STACK_HEADER_BIT_HAS_BEEN_ALLOCATED     31
#define EOS_STACK_HEADER_MASK_SIZE_READ             0x000000007FFFFFFF
#define EOS_STACK_HEADER_MASK_COUNT_WRITE           0x0000000080000000
#endif


EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class StackAllocator
{
public:
    EOS_INLINE StackAllocator()
    {
        Init(EOS_STACK_BLOCK_SIZE, EOS_STACK_BLOCK_COUNT);
    }

    EOS_INLINE ~StackAllocator()
    {
        Shutdown();
    }

    // in this version, the first time the allocation is called, set the size of the block
    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(_uiSize > 0, "Size must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment > 0, "Alignment must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be lesser or equal to the overall memory alignment", nullptr);
        eosAssertReturnValue(IsPowerOf2(_uiAlignment), "Alignment must be power of 2", nullptr);
        eosAssertReturnValue(m_uipStack, "Stack allocator is not allocated", nullptr);

        SharedMutexUniqueLock lock(m_memoryMutex);

        const eosSize uiMask = _uiAlignment - 1;
        const eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

        const eosSize uiNumBlocksNeed = 1 + ((uiSize - 1) / m_uiBlockSize);

        // find a free block
        eosBool bFoundBlock = false;
        eosSize uiBlock = 0;
        eosSize uiBlockCountUsed = 0;
        for (eosSize i = 0; i < m_uiBlockCount; ++i)
        {
            // there are a free block?
            const Header& pHeader = m_uipBlocks[i];

            const eosU8 uiHasBeenAllocated = EOS_BIT_CHECK(pHeader, EOS_STACK_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
            const eosSize uiCurrentBlockCount = EOS_BIT_GET(pHeader, EOS_STACK_HEADER_MASK_COUNT_READ);

            if (!uiHasBeenAllocated)
            {
                if (++uiBlockCountUsed == uiNumBlocksNeed)
                {
                    const eosSize uiOffset = (uiBlockCountUsed - 1);
                    const eosSize uiBlockIndex = i - uiOffset;

                    Header& pHeaderToUpdate = m_uipBlocks[uiBlockIndex];

                    EOS_BIT_SET(pHeaderToUpdate, EOS_STACK_HEADER_BIT_HAS_BEEN_ALLOCATED);
                    EOS_BIT_SET_VALUE(pHeaderToUpdate, EOS_STACK_HEADER_MASK_COUNT_WRITE, uiBlockCountUsed);

                    uiBlock = uiBlockIndex;
                    bFoundBlock = true;
                    break;
                }
            }
            else // no free adjacent block found but needs, so reset and restart
            {
                i += uiCurrentBlockCount;   // jump to the first free block
                uiBlockCountUsed = 0;
            }
        }

        eosAssertReturnValue(bFoundBlock && uiBlock < m_uiBlockCount, "There are any free block in the stack", nullptr);

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteAlloc(_uiSize, _uiAlignment, m_uiBlockSize * uiNumBlocksNeed, m_uipStack + (uiBlock * m_uiBlockSize));
#endif
        // return memory block
        return m_uipStack + (uiBlock * m_uiBlockSize);
    }

    EOS_INLINE void Free(void *_uipBuffer)
    {
        SharedMutexUniqueLock lock(m_memoryMutex);

        eosU8 *uipBuffer = static_cast<eosU8 *>(_uipBuffer);

        // check if this pointer is inside our list
        eosAssertReturnVoid(uipBuffer >= m_uipStack && uipBuffer < m_uipStack + (m_uiBlockSize * m_uiBlockCount), "Pointer is not in the stack");

        // convert block
        eosSize uiBlock = static_cast<eosSize>(uipBuffer - m_uipStack) / m_uiBlockSize;

        Header& pHeaderToUpdate = m_uipBlocks[uiBlock];

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        const eosSize uiCurrentBlockCount = EOS_BIT_GET(pHeaderToUpdate, EOS_STACK_HEADER_MASK_COUNT_READ);
        m_log.WriteFree((uiCurrentBlockCount * m_uiBlockSize), _uipBuffer);
#endif

        EOS_BIT_CLEAR(pHeaderToUpdate, EOS_STACK_HEADER_BIT_HAS_BEEN_ALLOCATED);
        EOS_BIT_SET_VALUE(pHeaderToUpdate, EOS_STACK_HEADER_MASK_COUNT_WRITE, 0);
    }


    EOS_INLINE void* Reallocate(void *_uipBuffer, eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(_uiAlignment > 0, "Alignment must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be lesser or equal to the overall memory alignment", nullptr);
        eosAssertReturnValue(_uiSize > 0, "Size must be passed greater then 0", nullptr);
        eosAssertReturnValue(IsPowerOf2(_uiAlignment), "Alignment must be power of 2", nullptr);

        void* ptr = nullptr;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        {
            m_log.WriteBeginRealloc();
#endif

            ptr = Alloc(_uiSize, _uiAlignment);

            {
                SharedMutexUniqueLock lock(m_memoryMutex);

                memcpy(ptr, _uipBuffer, _uiSize);
            }

            Free(_uipBuffer);

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
            m_log.WriteEndRealloc();
        }
#endif

        return ptr;  
    }

    EOS_INLINE eosBool IsPointerOwned(void *_uipBuffer)
    {
        return (_uipBuffer >= m_uipStack && _uipBuffer <= m_uipStack + (m_uiBlockSize * m_uiBlockCount));
    }

private:
    EOS_INLINE eosBool IsPowerOf2(eosSize _x)
    {
        return _x && !(_x & (_x - 1));
    }

    EOS_INLINE void Init(eosSize _uiBlockSize, eosSize _uiBlockCount)
    {
        eosAssertReturnVoid(!(_uiBlockSize % 2) && !(_uiBlockCount % 2), "Stack allocator: Block count and Block size must be multiple of 2!");

        m_uiBlockSize = _uiBlockSize;
        m_uiBlockCount = _uiBlockCount;

#ifdef EOS_x64
        m_uipBlocks = (Header*)calloc(m_uiBlockCount, sizeof(Header));
#else
        m_uipBlocks = (Header*)_aligned_malloc(m_uiBlockCount * sizeof(Header), EOS_MEMORY_ALIGNMENT_SIZE);
        memset(m_uipBlocks, 0, m_uiBlockCount * sizeof(Header));
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

private:
#ifdef EOS_x64
    typedef eosU64 Header;
#else
    typedef eosU32 Header;
#endif

    eosSize m_uiBlockSize;
    eosSize m_uiBlockCount;
    Header* m_uipBlocks;
    eosU8* m_uipStack;

    typedef std::shared_mutex               SharedMutex;
    typedef std::unique_lock<SharedMutex>   SharedMutexUniqueLock;

    SharedMutex m_memoryMutex;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
    Log m_log;
#endif
};

extern "C" EOS_DLL StackAllocator g_stackAllocator;

EOS_NAMESPACE_END
