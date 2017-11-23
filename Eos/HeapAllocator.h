#pragma once

#include "CoreDefs.h"

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
#include "Log.h"
#endif

#define EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED      31
#define EOS_HEAP_HEADER_BIT_STILL_IN_USE            30

#define EOS_HEAP_HEADER_MASK_SIZE_READ              0x000000003fffffff
#define EOS_HEAP_HEADER_MASK_SIZE_WRITE             0x00000000C0000000

#define EOS_HEAP_CONSTANT_HEADER_SIZE               16

EOS_NAMESPACE_BEGIN

EOS_MEMORY_ALIGNMENT(EOS_MEMORY_ALIGNMENT_SIZE) class HeapAllocator
{
public:
    EOS_INLINE void Init(eosSize _uiSize)
    {
        eosSize uiMask = EOS_MEMORY_ALIGNMENT_SIZE - 1;
        m_uiSize = (_uiSize + uiMask) & ~uiMask;

        m_uipHeap = (eosU8*)std::calloc(m_uiSize, sizeof(eosU8));
        eosAssert(m_uipHeap, "Memory is not allocated!");

        m_uipLast = m_uipHeap;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Init("heap.log", m_uiSize);
#endif
    }

    EOS_INLINE void Shutdown()
    {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
        m_log.Shutdown();
#endif
        std::free(m_uipHeap);

        m_uipHeap = nullptr;
        m_uipLast = nullptr;
        m_uiSize = 0;
    }


    // 1th bit of the header mean "has been allocated at least one time?"
    // 2nd bit of the header mean "it is currently allocated?"
    // rest of the header is the size
    EOS_INLINE void* Alloc(eosSize _uiSize, eosSize _uiAlignment)
    {
        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            eosAssertReturnValue(m_uipHeap, "Heap allocator is not allocated", nullptr);
            eosAssertReturnValue(_uiAlignment <= EOS_MEMORY_ALIGNMENT_SIZE, "Alignment must be less or equal to the allocator alignment", nullptr);

            eosSize uiMask = _uiAlignment - 1;
            eosSize uiSize = (_uiSize + uiMask) & ~uiMask;

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

            if (uiOldPtrFound)
            {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
                m_log.WriteAlloc(_uiSize, _uiAlignment, uiSize, uiCurrentSize, (uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE));
#endif
                return uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE;
            }
            else
            {
                uiCurrentSize = uiSize;

                EOS_BIT_SET(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED);
                EOS_BIT_SET(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);
                EOS_BIT_SET_VALUE(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_WRITE, uiCurrentSize);

                uipCurrentHeap += EOS_HEAP_CONSTANT_HEADER_SIZE;
                m_uipLast = uipCurrentHeap;

#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
                m_log.WriteAlloc(_uiSize, _uiAlignment, uiSize, uiSize, uipCurrentHeap);
#endif
                return uipCurrentHeap;
            }
        }
    }

    EOS_INLINE void Free(void *_uipBuffer)
    {
        {
            SharedMutexUniqueLock lock(m_memoryMutex);

            eosAssertDialog(_uipBuffer >= m_uipHeap && _uipBuffer <= m_uipLast);
            eosAssertReturnVoid(_uipBuffer >= m_uipHeap && _uipBuffer <= m_uipLast, "Pointer is not allocated in the heap");
            
            eosU8* uipCurrentHeap = m_uipHeap;

            Header* pHeader = reinterpret_cast<Header*>(uipCurrentHeap);

            eosU8* uipBuffer = uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE;

            eosU8 uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
            eosU8 uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
            eosSize uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);

            while (uiHasBeenAllocated)
            {
                if (uiStillInUse && uipBuffer == _uipBuffer)
                {
#if defined(_DEBUG) && defined(EOS_MEMORYLOAD)
                    m_log.WriteFree(uiCurrentSize, _uipBuffer);
#endif

                    EOS_BIT_CLEAR(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE);

                    break;  // early exit
                }
                else
                {
                    uipCurrentHeap += EOS_HEAP_CONSTANT_HEADER_SIZE + uiCurrentSize;

                    pHeader = reinterpret_cast<Header*>(uipCurrentHeap);

                    uipBuffer = uipCurrentHeap + EOS_HEAP_CONSTANT_HEADER_SIZE;


                    uiHasBeenAllocated = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_HAS_BEEN_ALLOCATED) > 0;
                    uiStillInUse = EOS_BIT_CHECK(*pHeader, EOS_HEAP_HEADER_BIT_STILL_IN_USE) > 0;
                    uiCurrentSize = EOS_BIT_GET(*pHeader, EOS_HEAP_HEADER_MASK_SIZE_READ);
                }
            }
        }
    }

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
    typedef eosU64 Header;

    eosU8* m_uipHeap;
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
