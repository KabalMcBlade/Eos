// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\Eos.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BasicDefines.h"
#include "Core/BasicTypes.h"
#include "Core/Assertions.h"
#include "Core/NoCopyable.h"
#include "Core/NumberUtils.h"
#include "Core/PointerUtils.h"

#include "DataStructures/LinkedList.h"
#include "DataStructures/StackLinkedList.h"
#include "DataStructures/DoublyLinkedList.h"

#include "MemoryBasicDefines.h"
#include "MemoryLayoutUtils.h"
#include "MemCpy.h"
#include "MemoryHeaderPolicy.h"
#include "MemoryThreadPolicy.h"
#include "MemoryAreaPolicy.h"
#include "MemoryAllocationPolicy.h"
#include "MemoryBoundsCheckPolicy.h"
#include "MemoryLogPolicy.h"
#include "MemoryTagPolicy.h"
#include "MemoryAllocator.h"
#include "SmartPointer.h"

#include "Allocators/LinearAllocator.h"
#include "Allocators/PoolAllocator.h"
#include "Allocators/FreeListAllocator.h"

#include "StlAllocator.h"
#include "StlAllocatorsTypes.h"

#include "MemoryFunctions.h"
