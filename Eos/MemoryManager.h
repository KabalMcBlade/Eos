#pragma once

#include "CoreDefs.h"

#include "HeapAllocator.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"


EOS_NAMESPACE_BEGIN

class MemoryManager
{
public:
    EOS_NO_INLINE static EOS_DLL MemoryManager& Instance();

    EOS_INLINE HeapAllocator& GetHeapAllocator()
    {
        return m_heapAllocator;
    }

    EOS_INLINE LinearAllocator& GetLinearAllocator()
    {
        return m_linearAllocator;
    }

    EOS_INLINE StackAllocator& GetStackAllocator()
    {
        return m_stackAllocator;
    }

private:
    MemoryManager() {}
    ~MemoryManager() {}

    MemoryManager(const MemoryManager& _Orig);          // disallow copy constructor
    MemoryManager& operator = (const MemoryManager&);   // disallow copy constructor

private:
    HeapAllocator    m_heapAllocator;
    LinearAllocator  m_linearAllocator;
    StackAllocator   m_stackAllocator;
};

EOS_NAMESPACE_END