#pragma once

#include "CoreDefs.h"
#include "MemoryManager.h"


EOS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeHeap(T *_Pointer)
{
    _Pointer->~T();
    MemoryManager::Instance().GetHeapAllocator().Free(_Pointer);
}

EOS_INLINE void FreeRawHeap(void *_Pointer)
{
    MemoryManager::Instance().GetHeapAllocator().Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocHeap(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (MemoryManager::Instance().GetHeapAllocator().Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeHeap<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawHeap(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return MemoryManager::Instance().GetHeapAllocator().Reallocate(_Pointer, _uiSize, _uiAlignment);
}


//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeLinear(T *_Pointer)
{
    _Pointer->~T();
    MemoryManager::Instance().GetLinearAllocator().Free(_Pointer);
}

EOS_INLINE void FreeRawLinear(void *_Pointer)
{
    MemoryManager::Instance().GetLinearAllocator().Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocLinear(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (MemoryManager::Instance().GetLinearAllocator().Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeLinear<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawLinear(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return MemoryManager::Instance().GetLinearAllocator().Reallocate(_Pointer, _uiSize, _uiAlignment);
}


//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeStack(T *_Pointer)
{
    _Pointer->~T();
    MemoryManager::Instance().GetStackAllocator().Free(_Pointer);
}

EOS_INLINE void FreeRawStack(void *_Pointer)
{
    MemoryManager::Instance().GetStackAllocator().Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocStack(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (MemoryManager::Instance().GetStackAllocator().Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeStack<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawStack(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return MemoryManager::Instance().GetStackAllocator().Reallocate(_Pointer, _uiSize, _uiAlignment);
}

EOS_NAMESPACE_END



// Put outside of scope using namespace scope when required in order to allow to use define without namespace scope

//////////////////////////////////////////////////////////////////////////
#define eosNew(Type, Alignment, ...)                    new ( eos::MemoryManager::Instance().GetHeapAllocator().Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRaw(Size, Alignment)                      eos::MemoryManager::Instance().GetHeapAllocator().Alloc(Size, Alignment)

#define eosDelete(Object)                               eos::FreeHeap(Object)
#define eosDeleteRaw(Object)                            eos::FreeRawHeap(Object)

#define eosRealloc(Object, Type, NewSize, Alignment)    eos::ReallocHeap<Type>(Object, NewSize, Alignment)
#define eosReallocRaw(Object, Size, Alignment)          eos::ReallocRawHeap(Object, Size, Alignment)


//////////////////////////////////////////////////////////////////////////
#define eosNewLinear(Type, Alignment, ...)              new ( eos::MemoryManager::Instance().GetLinearAllocator().Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRawLinear(Size, Alignment)                eos::MemoryManager::Instance().GetLinearAllocator().Alloc(Size, Alignment)

#define eosDeleteLinear(Object)                         eos::FreeLinear(Object)
#define eosDeleteRawLinear(Object)                      eos::FreeRawLinear(Object)

#define eosReallocLinear(Object, Type, Alignment)       eos::ReallocLinear<Type>(Object, sizeof(Type), Alignment)
#define eosReallocRawLinear(Object, Size, Alignment)    eos::ReallocRawLinear(Object, Size, Alignment)


//////////////////////////////////////////////////////////////////////////
#define eosNewStack(Type, Alignment, ...)               new ( eos::MemoryManager::Instance().GetStackAllocator().Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRawStack(Size, Alignment)                 eos::MemoryManager::Instance().GetStackAllocator().Alloc(Size, Alignment)

#define eosDeleteStack(Object)                          eos::FreeStack(Object)
#define eosDeleteRawStack(Object)                       eos::FreeRawStack(Object)

#define eosReallocStack(Object, Type, Alignment)        eos::ReallocStack<Type>(Object, sizeof(Type), Alignment)
#define eosReallocRawStack(Object, Size, Alignment)     eos::ReallocRawStack(Object, Size, Alignment)
