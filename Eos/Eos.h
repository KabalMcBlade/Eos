#pragma once

#include "CoreDefs.h"

#include "NonCopyable.h"

#include "VirtualMemory.h"

#include "MemoryUtils.h"
#include "MemoryAllocationHeaderPolicy.h"
#include "MemoryTrackingPolicy.h"
#include "MemoryThreadPolicy.h"
#include "MemoryTaggingPolicy.h"
#include "MemoryBoundsCheckingPolicy.h"
#include "MemoryAreaPolicy.h"
#include "MemoryAllocationPolicy.h"
#include "MemoryFreeList.h"
#include "MemoryBuddyAllocator.h"

#include "LinearAllocator.h"
#include "PoolAllocator.h"
#include "StackFromBottomAllocatorPolicy.h"
#include "StackFromTopAllocatorPolicy.h"
#include "StackAllocator.h"
#include "HeapAllocator.h"
#include "MallocAllocator.h"

#include "Allocator.h"

#include "MemoryFunctions.h"
#include "SmartPointer.h"
#include "StlAllocator.h"
#include "StlAllocatorsTypes.h"
