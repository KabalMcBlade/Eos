#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "Eos/Eos.h"

EOS_USING_NAMESPACE

struct Cat
{
	static constexpr eosSize kSize = 1 << 8;	// ~256 bytes
    eosU8 m_buffer[kSize];
};

/*
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
*/

int main()
{
	std::cout 
		<< "TESTING VARIOUS ALLOCATORS" 
		<< std::endl 
		<< "Allocate and Deallocate 8192 Objects of ~256 bytes each, because of this the system is using the Heap Area or the Dynamic Heap Area policies for the allocator" 
		<< std::endl 
		<< "NOTE: To see the performance you need to run in Release or any of the optimized builds"
		<< std::endl 
		<< std::endl
		<< "The next tests are using dynamic new/delete provided by EOS"
		<< std::endl
		<< std::endl;

	static constexpr eosS32 kBufferExtensionMultiplier = 8;
	static constexpr eosS32 kCount = 8192;
	static constexpr eosS32 kBufferSize = kCount * (sizeof(Cat));



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Cat* v[kCount];
	eosS32 i = 0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	EOS_PROFILE_START(Original_New_and_Delete);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = new Cat;
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			delete v[i];
			++i;
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosDynamicHeapArea mallocAreaMemory(sizeof(Cat) * kBufferExtensionMultiplier, kBufferSize * kBufferExtensionMultiplier);
	using MallocAllocator = eosAllocator<eosDefaultMallocAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	MallocAllocator mallocAllocator(mallocAreaMemory, "MallocAllocator");
	EOS_PROFILE_START(EOS_Malloc_Allocator);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &mallocAllocator);
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			eosDelete(v[i], &mallocAllocator);
			++i;
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	static constexpr eosSize kOrderLevel = 22;		// ~4MB
	static constexpr eosSize kHeapSize = 1 << kOrderLevel;
	eosHeapArea heapAreaMemory(kHeapSize);
	using HeapAllocator = eosAllocator<eosDefaultHeapAllocationPolicy<EOS_MEMORY_ALIGNMENT_SIZE, kOrderLevel>, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	HeapAllocator heapAllocator(heapAreaMemory, "HeapAllocator");
	EOS_PROFILE_START(EOS_Heap_Allocator);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &heapAllocator);
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			eosDelete(v[i], &heapAllocator);
			++i;
		}
	}
	EOS_PROFILE_END;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosHeapArea linearAreaMemory(kBufferSize * kBufferExtensionMultiplier);
	using LinearAllocator = eosAllocator<eosDefaultLinearAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	LinearAllocator linearAllocator(linearAreaMemory, "LinearAllocator");
	EOS_PROFILE_START(EOS_Linear_Allocator);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &linearAllocator);
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			eosDelete(v[i], &linearAllocator);
			++i;
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosHeapArea poolHeapAreaMemoryNoGrowable(kBufferSize * kBufferExtensionMultiplier);
	using PoolAllocatorNoGrowable = eosAllocator<eosDefaultPoolNoGrowableAllocationPolicy<sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE>, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	PoolAllocatorNoGrowable poolAllocatorNoGrowable(poolHeapAreaMemoryNoGrowable, "PoolAllocator_NoGrowable");
	EOS_PROFILE_START(EOS_Pool_Allocator_NO_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &poolAllocatorNoGrowable);
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			eosDelete(v[i], &poolAllocatorNoGrowable);
			++i;
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosDynamicHeapArea poolHeapAreaMemoryGrowable(sizeof(Cat) * kBufferExtensionMultiplier, kBufferSize * kBufferExtensionMultiplier);
	using PoolAllocatorGrowable = eosAllocator<eosDefaultPoolGrowableAllocationPolicy<sizeof(Cat), EOS_MEMORY_ALIGNMENT_SIZE, sizeof(Cat) * kBufferExtensionMultiplier>, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	PoolAllocatorGrowable poolAllocatorGrowable(poolHeapAreaMemoryGrowable, "PoolAllocator_Growable");
	EOS_PROFILE_START(EOS_Pool_Allocator_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &poolAllocatorGrowable);
			++i;
		}

		i = 0;
		while (i < kCount)
		{
			eosDelete(v[i], &poolAllocatorGrowable);
			++i;
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosHeapArea stackTopHeapAreaMemoryNoGrowable(kBufferSize * kBufferExtensionMultiplier);
	using StackFromTopNoGrowable = eosAllocator<eosDefaultStackTopNoGrowableAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	StackFromTopNoGrowable stackAllocatorFromTopNoGrowable(stackTopHeapAreaMemoryNoGrowable, "StackAllocator_FromTop_NoGrowable");
	EOS_PROFILE_START(EOS_Stack_From_Top_Allocator_NO_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &stackAllocatorFromTopNoGrowable);
			++i;
		}

		i = kCount;
		while (i > 0)
		{
			--i;
			eosDelete(v[i], &stackAllocatorFromTopNoGrowable);
		}
	}
	EOS_PROFILE_END;

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosDynamicHeapArea stackTopHeapAreaMemoryGrowable(sizeof(Cat) * kBufferExtensionMultiplier, kBufferSize * kBufferExtensionMultiplier);
	using StackFromTopGrowable = eosAllocator<eosDefaultStackTopGrowableAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	StackFromTopGrowable stackAllocatorFromTopGrowable(stackTopHeapAreaMemoryGrowable, "StackAllocator_FromTop_Growable");
	EOS_PROFILE_START(EOS_Stack_From_Top_Allocator_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &stackAllocatorFromTopGrowable);
			++i;
		}

		i = kCount;
		while (i > 0)
		{
			--i;
			eosDelete(v[i], &stackAllocatorFromTopGrowable);
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosHeapArea stackBottomHeapAreaMemoryNoGrowable(kBufferSize * kBufferExtensionMultiplier);
	using StackFromBottomNoGrowable = eosAllocator<eosDefaultStackBottomNoGrowableAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	StackFromBottomNoGrowable stackAllocatorFromBottomNoGrowable(stackBottomHeapAreaMemoryNoGrowable, "StackAllocator_FromBottom_NoGrowable");
	EOS_PROFILE_START(EOS_Stack_From_Bottom_Allocator_NO_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &stackAllocatorFromBottomNoGrowable);
			++i;
		}

		i = kCount;
		while (i > 0)
		{
			--i;
			eosDelete(v[i], &stackAllocatorFromBottomNoGrowable);
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	eosDynamicHeapArea stackBottomHeapAreaMemoryGrowable(sizeof(Cat) * kBufferExtensionMultiplier, kBufferSize * kBufferExtensionMultiplier);
	using StackFromBottomGrowable = eosAllocator<eosDefaultStackBottomGrowableAllocationPolicy, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
	StackFromBottomGrowable stackAllocatorFromBottomGrowable(stackBottomHeapAreaMemoryGrowable, "StackAllocator_FromBottom_Growable");
	EOS_PROFILE_START(EOS_Stack_From_Bottom_Allocator_Growable);
	{
		i = 0;
		while (i < kCount)
		{
			v[i] = eosNew(Cat, &stackAllocatorFromBottomGrowable);
			++i;
		}

		i = kCount;
		while (i > 0)
		{
			--i;
			eosDelete(v[i], &stackAllocatorFromBottomGrowable);
		}
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	std::cout
		<< std::endl
		<< std::endl
		<< "The next tests are using array function, fixed or dynamic, provided by EOS, some of them, just to give an idea"
		<< std::endl
		<< std::endl;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	MallocAllocator mallocAllocatorFixedArray(mallocAreaMemory, "Fixed_Array_MallocAllocator");

	EOS_PROFILE_START(EOS_FIXED_ARRAY_Malloc_Allocator);
	{
		Cat* arr = eosNewArray(Cat[kCount], &mallocAllocatorFixedArray);
		eosDeleteArray(arr, &mallocAllocatorFixedArray);
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	MallocAllocator mallocAllocatorArray(mallocAreaMemory, "Dynamic_Array_MallocAllocator");

	EOS_PROFILE_START(EOS_DYNAMIC_ARRAY_Malloc_Allocator);
	{
		Cat* arr = eosNewDynamicArray(Cat, kCount, &mallocAllocatorArray);
		eosDeleteArray(arr, &mallocAllocatorArray);
	}
	EOS_PROFILE_END;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	HeapAllocator heapAllocatorFixedArray(linearAreaMemory, "Fixed_Array_HeapAllocator");

	EOS_PROFILE_START(EOS_FIXED_ARRAY_Heap_Allocator);
	{
		Cat* arr = eosNewArray(Cat[kCount], &heapAllocatorFixedArray);
		eosDeleteArray(arr, &heapAllocatorFixedArray);
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	HeapAllocator heapAllocatorArray(linearAreaMemory, "Dynamic_Array_HeapAllocator");

	EOS_PROFILE_START(EOS_DYNAMIC_ARRAY_Heap_Allocator);
	{
		Cat* arr = eosNewDynamicArray(Cat, kCount, &heapAllocatorArray);
		eosDeleteArray(arr, &heapAllocatorArray);
	}
	EOS_PROFILE_END;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	LinearAllocator linearAllocatorFixedArray(linearAreaMemory, "Fixed_Array_LinearAllocator");

	EOS_PROFILE_START(EOS_FIXED_ARRAY_Linear_Allocator);
	{
		Cat* arr = eosNewArray(Cat[kCount], &linearAllocatorFixedArray);
		eosDeleteArray(arr, &linearAllocatorFixedArray);
	}
	EOS_PROFILE_END;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	LinearAllocator linearAllocatorArray(linearAreaMemory, "Dynamic_Array_LinearAllocator");

	EOS_PROFILE_START(EOS_DYNAMIC_ARRAY_Linear_Allocator);
	{
		Cat* arr = eosNewDynamicArray(Cat, kCount, &linearAllocatorArray);
		eosDeleteArray(arr, &linearAllocatorArray);
	}
	EOS_PROFILE_END;


	return 0;

// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	PoolAllocatorNoGrowable poolAllocatorNoGrowableFixedArray(poolHeapAreaMemoryNoGrowable, "Fixed_Array_PoolAllocator_NoGrowable");
// 
// 	EOS_PROFILE_START(EOS_FIXED_ARRAY_Pool_Allocator_NO_Growable);
// 	{
// 		Cat* arr = eosNewArray(Cat[kCount], &poolAllocatorNoGrowableFixedArray);
// 		eosDeleteArray(arr, &poolAllocatorNoGrowableFixedArray);
// 	}
// 	EOS_PROFILE_END;
// 
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	PoolAllocatorNoGrowable poolAllocatorNoGrowableArray(poolHeapAreaMemoryNoGrowable, "Dynamic_Array_PoolAllocator_NoGrowable");
// 
// 	EOS_PROFILE_START(EOS_DYNAMIC_ARRAY_Pool_Allocator_NO_Growable);
// 	{
// 		Cat* arr = eosNewDynamicArray(Cat, kCount, &poolAllocatorNoGrowableArray);
// 		eosDeleteArray(arr, &poolAllocatorNoGrowableArray);
// 	}
// 	EOS_PROFILE_END;
// 
// 
// 
// 
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// 
// 	std::cout
// 		<< std::endl
// 		<< std::endl
// 		<< "The next tests are using STL provided by EOS, some of them, just to give an idea"
// 		<< std::endl
// 		<< std::endl;
// 
// 
// 
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	LinearAllocator stlLinearAllocator(linearAreaMemory, "STL_LinearAllocator");
// 	eosVector<Cat, LinearAllocator> linearVector(&stlLinearAllocator);
// 
// 	EOS_PROFILE_START(EOS_STL_VECTOR_Linear_Allocator);
// 	{
// 		linearVector.resize(kCount);
// 		linearVector.clear();
// 	}
// 	EOS_PROFILE_END;
// 
// 
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	PoolAllocatorNoGrowable stlNoGrowableAllocator(poolHeapAreaMemoryNoGrowable, "STL_Pool_NO_Growable_Allocator");
// 	eosVector<Cat, PoolAllocatorNoGrowable> poolNoGrowableVector(&stlNoGrowableAllocator);
// 
// 	EOS_PROFILE_START(EOS_STL_VECTOR_Pool_NO_Growable_Allocator);
// 	{
// 		poolNoGrowableVector.resize(kCount);
// 		poolNoGrowableVector.clear();
// 	}
// 	EOS_PROFILE_END;
// 
//     return 0;
}

