#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "Eos/Eos.h"


#define MEMORY_1_MB                 1048576
#define MEMORY_4_MB                 MEMORY_1_MB * 4
#define MEMORY_8_MB                 MEMORY_4_MB * 2
#define MEMORY_16_MB                MEMORY_8_MB * 2

#define STL_MAX_HEAP_MEMORY         MEMORY_16_MB
#define STL_MAX_STACK_MEMORY_SIZE   MEMORY_4_MB
#define STL_MAX_LINEAR_MEMORY       MEMORY_8_MB


#define MAX_STACK_MEMORY_BLOCK      1024
#define ALL_HEAP_MEMORY             STL_MAX_HEAP_MEMORY + (MEMORY_16_MB)
#define ALL_LINEAR_MEMORY           STL_MAX_LINEAR_MEMORY + (MEMORY_16_MB * 2)
#define ALL_STACK_MEMORY            STL_MAX_STACK_MEMORY_SIZE


EOS_USING_NAMESPACE

EOS_OPTIMIZATEOS_OFF

class Cat
{
public:
    int a;

    Cat()
    {
        a = 0xABCD;
    }

    Cat(const Cat& other)
    {
        a = other.a;
    }

    Cat& operator=(const Cat& other)
    {
        a = other.a;
        return *this;
    }

    Cat(int _a)
    {
        a = _a;
    }

    virtual ~Cat()
    {
        a = 0x0000;
    }
};

class SmartCat : public eosSmartObject
{
public:
    int a;

    SmartCat()
    {
        a = 0xCAFE;
    }

    SmartCat(const SmartCat& other)
    {
        a = other.a;
    }

    SmartCat& operator=(const SmartCat& other)
    {
        a = other.a;
        return *this;
    }

    SmartCat(int _a)
    {
        a = _a;
    }

    virtual ~SmartCat()
    {
        a = 0x0000;
    }
};

class SuperCat : public Cat
{
public:
    char counter[10];
    int numlol;

    SuperCat() : Cat()
    {
        memcpy(counter, "xxxxxxxxxx", 10);
        numlol = 101;
    }

    SuperCat(int _a) : Cat(_a)
    {
        memcpy(counter, "0123456789", 10);
    }

    virtual ~SuperCat()
    {
        numlol = 0;
        memset(counter, 0, 10);
    }
};


int main()
{
    std::cout << "TESTING VARIOUS ALLOCATORS" << std::endl << "Allocate and Deallocate 4096 Objects" << std::endl << std::endl;

    static constexpr eosS32 kCatCount = 4096;
    static constexpr eosS32 kCatBufferForStaticAllocator = kCatCount * (sizeof(Cat));

    Cat* v[kCatCount];
    eosS32 i = 0;


    //eosFixedStackArea<kCatBufferForStaticAllocator> areakMemory;
    eosHeapArea linearAreaMemory(kCatBufferForStaticAllocator * 32);
    using CatLinearAllocator = eosAllocator<eosDefaultLinearAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatLinearAllocator linearAllocator(linearAreaMemory, "LinearAllocator");
    EOS_PROFILE_START(eos_linear_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(linearAllocator.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = 0;
        while (i < kCatCount)
        {
            linearAllocator.Free(v[i]);
            ++i;
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosHeapArea poolHeapAreaMemory(kCatBufferForStaticAllocator * 32);
    using CatPoolAllocatorNonGrowable = eosPoolAllocatorNonGrowable<sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE>;
    using CatPoolAllocateosPolicyNonGrowable = eosAllocationPolicy<CatPoolAllocatorNonGrowable, eosAllocationHeaderU32>;
    using CatNonGrowablePoolAllocator = eosAllocator<CatPoolAllocateosPolicyNonGrowable, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatNonGrowablePoolAllocator poolAllocator(poolHeapAreaMemory, "PoolAllocator_NonGrowable");
    EOS_PROFILE_START(eos_pool_non_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(poolAllocator.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = 0;
        while (i < kCatCount)
        {
            poolAllocator.Free(v[i]);
            ++i;
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosDynamicHeapArea poolHeapGrowableAreaMemory(1024, kCatBufferForStaticAllocator * 32);
    using CatPoolAllocatorGrowable = eosPoolAllocatorGrowable<sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, false, sizeof(Cat) * 32>;
    using CatPoolAllocateosPolicyGrowable = eosAllocationPolicy<CatPoolAllocatorGrowable, eosAllocationHeaderU32>;
    using CatGrowablePoolAllocator = eosAllocator<CatPoolAllocateosPolicyGrowable, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatGrowablePoolAllocator growablePoolAllocator(poolHeapGrowableAreaMemory, "PoolAllocator_Growable");
    EOS_PROFILE_START(eos_pool_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(growablePoolAllocator.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = 0;
        while (i < kCatCount)
        {
            growablePoolAllocator.Free(v[i]);
            ++i;
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosHeapArea stackTopHeapAreaMemory(kCatBufferForStaticAllocator * 32);
    using CaStackAllocateosPolicyNonGrowableFromTop = eosAllocationPolicy<eosStackAllocatorTopNonGrowable, eosAllocationHeaderU32>;
    using CatStackFromTopNonGrowable = eosAllocator<CaStackAllocateosPolicyNonGrowableFromTop, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatStackFromTopNonGrowable stackAllocatorFromTop(stackTopHeapAreaMemory, "StackAllocator_FromTop_NonGrowable");
    EOS_PROFILE_START(eos_stack_from_top_non_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(stackAllocatorFromTop.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = kCatCount;
        while (i > 0)
        {
            --i;
            stackAllocatorFromTop.Free(v[i]);
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosDynamicHeapArea stackTopHeapAreaMemoryGrowable(1024, kCatBufferForStaticAllocator * 32);
    using CaStackAllocateosPolicyGrowableFromTop = eosAllocationPolicy<eosStackAllocatorTopGrowable, eosAllocationHeaderU32>;
    using CatStackFromTopGrowable = eosAllocator<CaStackAllocateosPolicyGrowableFromTop, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatStackFromTopGrowable stackAllocatorFromTopGrowable(stackTopHeapAreaMemoryGrowable, "StackAllocator_FromTop_Growable");
    EOS_PROFILE_START(eos_stack_from_top_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(stackAllocatorFromTopGrowable.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = kCatCount;
        while (i > 0)
        {
            --i;
            stackAllocatorFromTopGrowable.Free(v[i]);
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosHeapArea stackBottomHeapAreaMemory(kCatBufferForStaticAllocator * 32);
    using CaStackAllocateosPolicyNonGrowableFromBottom = eosAllocationPolicy<eosStackAllocatorBottomNonGrowable, eosAllocationHeaderU32>;
    using CatStackFromBottomNonGrowable = eosAllocator<CaStackAllocateosPolicyNonGrowableFromBottom, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatStackFromBottomNonGrowable stackAllocatorFromBottom(stackBottomHeapAreaMemory, "StackAllocator_FromBottom_NonGrowable");
    EOS_PROFILE_START(eos_stack_from_bottom_non_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(stackAllocatorFromBottom.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = kCatCount;
        while (i > 0)
        {
            --i;
            stackAllocatorFromBottom.Free(v[i]);
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosDynamicHeapArea stackBottomHeapAreaMemoryGrowable(1024, kCatBufferForStaticAllocator * 32);
    using CaStackAllocateosPolicyGrowableFromBottom = eosAllocationPolicy<eosStackAllocatorBottomGrowable, eosAllocationHeaderU32>;
    using CatStackFromBottomGrowable = eosAllocator<CaStackAllocateosPolicyGrowableFromBottom, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatStackFromBottomGrowable stackAllocatorFromBottomGrowable(stackBottomHeapAreaMemoryGrowable, "StackAllocator_FromBottom_Growable");
    EOS_PROFILE_START(eos_stack_from_bottom_growable_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(stackAllocatorFromBottomGrowable.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = kCatCount;
        while (i > 0)
        {
            --i;
            stackAllocatorFromBottomGrowable.Free(v[i]);
        }
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
//     const eosBool result = eosVirtualMemory::EnableLargePageSupport();
//     if (result)
//     {
//         eosSize hugeSize = 100000000000;
//         hugeSize = eosBitUtils::RoundUpToMultiple(hugeSize, eosVirtualMemory::GetLargePageSize());
// 
//         eosSize chunkSize = 10000000;
//         chunkSize = eosBitUtils::RoundUpToMultiple(chunkSize, eosVirtualMemory::GetLargePageSize());
// 
//         void* X = eosVirtualMemory::ReserveAddressSpace(hugeSize, true);
//         eosVirtualMemory::CommitPhysicalMemory(X, chunkSize, true);
//     }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosEmptyArea heapAllocatorArea;
    using CaHeapAllocateosPolicy = eosAllocationPolicy<eosHeapAllocator, eosAllocationHeaderU32>;
    using CatHeap = eosAllocator<CaHeapAllocateosPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
    CatHeap heapAllocator(heapAllocatorArea, "HeapAllocator_NonGrowable");

    EOS_PROFILE_START(eos_heap_allocator);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = static_cast<Cat*>(heapAllocator.Allocate(sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO));
            ++i;
        }

        i = 0;
        while (i < kCatCount)
        {
            heapAllocator.Free(v[i]);
            ++i;
        }
    }
    EOS_PROFILE_END;



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    CatLinearAllocator arrayLinearAllocator(linearAreaMemory, "Array_LinearAllocator");

    EOS_PROFILE_START(eos_array_linear_allocator);
    {
        Cat* arr = eosNewArray(Cat[kCatCount], &arrayLinearAllocator);
        eosDeleteArray(arr, &arrayLinearAllocator);
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    CatLinearAllocator dynamicArrayLinearAllocator(linearAreaMemory, "Dyamic_Array_LinearAllocator");

    eosU32 counter = kCatCount;
    EOS_PROFILE_START(eos_dynamic_array_linear_allocator);
    {
        Cat* arr = eosNewDynamicArray(Cat, counter, &dynamicArrayLinearAllocator);
        eosDeleteArray(arr, &dynamicArrayLinearAllocator);
    }
    EOS_PROFILE_END;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    CatLinearAllocator stlLinearAllocator(linearAreaMemory, "STL_LinearAllocator");
    eosVector<Cat, CatLinearAllocator> catVector(&stlLinearAllocator);

    EOS_PROFILE_START(eos_stl_vector_linear_allocator);
    {
        catVector.resize(kCatCount);
        catVector.clear();
    }
    EOS_PROFILE_END;



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    EOS_PROFILE_START(new_delete);
    {
        i = 0;
        while (i < kCatCount)
        {
            v[i] = new Cat;
            ++i;
        }

        i = 0;
        while (i < kCatCount)
        {
            delete v[i];
            ++i;
        }
    }
    EOS_PROFILE_END;



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    eosDynamicHeapArea smartPoolHeapGrowableAreaMemory(1024, kCatBufferForStaticAllocator * 32);
    using SmartCatPoolAllocatorGrowable = eosPoolAllocatorGrowable<sizeof(SmartCat), EOS_MEMORY_ALIGNMENT_SIZE, false, sizeof(SmartCat) * 32>;
    using SmartCatPoolAllocateosPolicyGrowable = eosAllocationPolicy<SmartCatPoolAllocatorGrowable, eosAllocationHeaderU32>;
    using SmartCatGrowablePoolAllocator = eosAllocator<SmartCatPoolAllocateosPolicyGrowable, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;

    SmartCatGrowablePoolAllocator smartGrowablePoolAllocator(smartPoolHeapGrowableAreaMemory, "Smart_PoolAllocator_Growable");
    {
        eosSmartPointer<SmartCat, SmartCatGrowablePoolAllocator> catSmart(&smartGrowablePoolAllocator);

        eosSmartPointer<SmartCat, SmartCatGrowablePoolAllocator> catSmartClone1 = catSmart;
        eosSmartPointer<SmartCat, SmartCatGrowablePoolAllocator> catSmartClone2 = catSmart;
        eosSmartPointer<SmartCat, SmartCatGrowablePoolAllocator> catSmartClone3 = catSmart;
    }


    return 0;
}

