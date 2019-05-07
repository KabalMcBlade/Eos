#pragma once

#include "CoreDefs.h"
#include "MemoryDefines.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

#ifdef EOS_x64
#define EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED     63
#define EOS_HEAP_HEADER_BIT_STILL_IN_USE           62
#define EOS_HEAP_HEADER_MASK_SIZE_READ             0x3FFFFFFFFFFFFFFF
#define EOS_HEAP_HEADER_MASK_SIZE_WRITE            0xC000000000000000
#else
#define EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED     31
#define EOS_HEAP_HEADER_BIT_STILL_IN_USE           30
#define EOS_HEAP_HEADER_MASK_SIZE_READ             0x000000003FFFFFFF
#define EOS_HEAP_HEADER_MASK_SIZE_WRITE            0x00000000C0000000
#endif

#define EOS_HEAP_CONSTANT_HEADER_SIZE              EOS_MEMORY_ALIGNMENT_SIZE

EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class HeapAllocator
{
public:
    EOS_INLINE HeapAllocator()
    {
        Init(EOS_HEAP_MEMORY);
    }

    EOS_INLINE ~HeapAllocator()
    {
        Shutdown();
    }

    // 1th bit of the header mean "has been allocated at least one time?"
    // 2nd bit of the header mean "it is currently allocated?"
    // rest of the header is the size
    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(m_uipHeap, "Heap allocator is not allocated", nullptr);
        eosAssertReturnValue(_uiSize > 0, "Size must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment > 0, "Alignment must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be lesser or equal to the overall memory alignment", nullptr);
        eosAssertReturnValue(IsPowerOf2(_uiAlignment), "Alignment must be power of 2", nullptr);

        SharedMutexUniqueLock lock(m_memoryMutex);

        const eosSize uiMask = _uiAlignment - 1;
        const eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

        eosU8* uipCurrentHeap = m_uipHeap;

        Header* pHeader = reinterpret_cast<Header*>(uipCurrentHeap);

        eosU8 uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
        eosU8 uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
        eosSize uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);

        eosU8 uiOldPtrFound = 0;

        while (uiHasBeenAllocated && !uiOldPtrFound)
        {
            // if not in use and the new size is less than last I can place new one pointer here (size must still the same)
            if (!uiStillInUse && uiCurrentSize >= uiSize)
            {
                // set it is allocated again:
                EOS_BIT_SET(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);
                uiOldPtrFound = 1;
                break;  // early exit
            }
            else
            {
                // increase the pointer location, get value and continue
                uipCurrentHeap += EOS_HEAP_CONSTANT_HEADER_SIZE + uiCurrentSize;

                pHeader = reinterpret_cast<Header*>(uipCurrentHeap);

                uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
                uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
                uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);
            }
        }

        eosAssertReturnValue(uipCurrentHeap >= m_uipHeap && uipCurrentHeap <= m_uipHeap + m_uiSize * sizeof(eosU8), "Pointer is out of bound of the heap", nullptr);

        if (!uiOldPtrFound)
        {
            uiCurrentSize = uiSize;

            EOS_BIT_SET(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED);
            EOS_BIT_SET(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);
            EOS_BIT_SET_VALUE(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_WRITE, uiCurrentSize);
        }

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.WriteAlloc(_uiSize, _uiAlignment, uiCurrentSize, (uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE));
#endif

        return uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE;
    }

    EOS_INLINE void Free(void *_uipBuffer)
    {
        eosAssertDialog(_uipBuffer >= m_uipHeap && _uipBuffer <= m_uipHeap + m_uiSize);
        eosAssertReturnVoid(_uipBuffer >= m_uipHeap && _uipBuffer <= m_uipHeap + m_uiSize, "Pointer is not allocated in the heap");

        SharedMutexUniqueLock lock(m_memoryMutex);

        eosU8* uipBuffer = reinterpret_cast<eosU8*>(_uipBuffer);

        Header* pHeader = reinterpret_cast<Header*>(uipBuffer - EOS_HEAP_CONSTANT_HEADER_SIZE);

        eosU8 uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
        eosU8 uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
        eosSize uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);

        if (uiHasBeenAllocated && uiStillInUse)
        {
            EOS_BIT_CLEAR(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
            m_log.WriteFree(uiCurrentSize, _uipBuffer);
#endif
        }

#ifdef EOS_HEAP_MEMORY_USE_BUDDY
        eosSize uiInitialBlocksSize = uiCurrentSize;

        Header* pInitialHeader = pHeader;

        eosSize uiTotalEmptyBlocksSize = 0;

        eosU8 uiExitFlag = 0;

        // this initially point after the header, but because in the while I have to add, I'm removing now
        uipBuffer -= EOS_HEAP_CONSTANT_HEADER_SIZE;

        // while was allocated but is not in use anymore and is a next block of memory of course
        while (!uiExitFlag)
        {
            uipBuffer += EOS_HEAP_CONSTANT_HEADER_SIZE + uiCurrentSize;

            pHeader = reinterpret_cast<Header*>(uipBuffer);

            uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
            uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
            uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);

            if (uiHasBeenAllocated && !uiStillInUse)
            {
                uiTotalEmptyBlocksSize += EOS_HEAP_CONSTANT_HEADER_SIZE + uiCurrentSize;
                EOS_BIT_CLEAR(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED);
                EOS_BIT_CLEAR(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);
                EOS_BIT_SET_VALUE(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_WRITE, 0);
            }
            else
            {
                uiExitFlag = 1;
            }
        }

        EOS_BIT_SET_VALUE(*pInitialHeader, EOS_HEAP_HEADER_MASK_SIZE_WRITE, uiInitialBlocksSize + uiTotalEmptyBlocksSize);
#endif // EOS_HEAP_MEMORY_USE_BUDDY
    }

    EOS_INLINE void* Reallocate(void *_uipBuffer, eosSize _uiSize, eosSize _uiAlignment)
    {
        eosAssertReturnValue(_uiSize > 0, "Size must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment > 0, "Alignment must be passed greater then 0", nullptr);
        eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be lesser or equal to the overall memory alignment", nullptr);
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
        return (_uipBuffer >= m_uipHeap && _uipBuffer <= m_uipHeap + m_uiSize);
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
        m_uipHeap = (eosU8*)calloc(m_uiSize, sizeof(eosU8));
#else
        m_uipHeap = (eosU8*)_aligned_malloc(m_uiSize * sizeof(eosU8), EOS_MEMORY_ALIGNMENT_SIZE);
        memset(m_uipHeap, 0, m_uiSize * sizeof(eosU8));
#endif // EOS_x64

        eosAssert(m_uipHeap, "Memory is not allocated!");

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Init("heap.log", m_uiSize);
#endif
    }

    EOS_INLINE void Shutdown()
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Shutdown();
#endif

#ifdef EOS_x64
        free(m_uipHeap);
#else
        _aligned_free(m_uipHeap);
#endif // EOS_x64

        m_uipHeap = nullptr;
        m_uiSize = 0;
    }


private:
#ifdef EOS_x64
    typedef eosU64 Header;
#else
    typedef eosU32 Header;
#endif

    eosU8* m_uipHeap;
    eosSize m_uiSize;

    typedef std::shared_mutex               SharedMutex;
    typedef std::unique_lock<SharedMutex>   SharedMutexUniqueLock;

    SharedMutex m_memoryMutex;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
    Log m_log;
#endif
};

extern "C" EOS_DLL HeapAllocator g_heapAllocator;

EOS_NAMESPACE_END
