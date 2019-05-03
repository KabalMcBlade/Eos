#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "HeapAllocator.h"

EOS_NAMESPACE_BEGIN

LinearAllocator g_linearAllocator;
StackAllocator g_stackAllocator;
HeapAllocator g_heapAllocator;

EOS_NAMESPACE_END