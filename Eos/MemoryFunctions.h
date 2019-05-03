#pragma once

#include "CoreDefs.h"

#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "HeapAllocator.h"

EOS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeHeap(T *_Pointer)
{
    _Pointer->~T();
    g_heapAllocator.Free(_Pointer);
}

EOS_INLINE void FreeRawHeap(void *_Pointer)
{
    g_heapAllocator.Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocHeap(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (g_heapAllocator.Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeHeap<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawHeap(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return g_heapAllocator.Reallocate(_Pointer, _uiSize, _uiAlignment);
}


//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeLinear(T *_Pointer)
{
    _Pointer->~T();
    g_linearAllocator.Free(_Pointer);
}

EOS_INLINE void FreeRawLinear(void *_Pointer)
{
    g_linearAllocator.Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocLinear(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (g_linearAllocator.Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeLinear<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawLinear(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return g_linearAllocator.Reallocate(_Pointer, _uiSize, _uiAlignment);
}


//////////////////////////////////////////////////////////////////////////
template<typename T>
EOS_INLINE void FreeStack(T *_Pointer)
{
    _Pointer->~T();
    g_stackAllocator.Free(_Pointer);
}

EOS_INLINE void FreeRawStack(void *_Pointer)
{
    g_stackAllocator.Free(_Pointer);
}

template<typename T>
EOS_INLINE T* ReallocStack(T *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    T* newPtr = new (g_stackAllocator.Alloc(_uiSize, _uiAlignment)) T;
    memcpy(newPtr, _Pointer, _uiSize);
    FreeStack<T>(_Pointer);
    return newPtr;
}

EOS_INLINE void* ReallocRawStack(void *_Pointer, eosSize _uiSize, eosSize _uiAlignment)
{
    return g_stackAllocator.Reallocate(_Pointer, _uiSize, _uiAlignment);
}

EOS_NAMESPACE_END



// Put outside of scope using namespace scope when required in order to allow to use define without namespace scope

//////////////////////////////////////////////////////////////////////////
#define eosNew(Type, Alignment, ...)                    new ( eos::g_heapAllocator.Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRaw(Size, Alignment)                      eos::g_heapAllocator.Alloc(Size, Alignment)

#define eosDelete(Object)                               eos::FreeHeap(Object)
#define eosDeleteRaw(Object)                            eos::FreeRawHeap(Object)

#define eosRealloc(Object, Type, NewSize, Alignment)    eos::ReallocHeap<Type>(Object, NewSize, Alignment)
#define eosReallocRaw(Object, Size, Alignment)          eos::ReallocRawHeap(Object, Size, Alignment)


//////////////////////////////////////////////////////////////////////////
#define eosNewLinear(Type, Alignment, ...)              new ( eos::g_linearAllocator.Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRawLinear(Size, Alignment)                eos::g_linearAllocator.Alloc(Size, Alignment)

#define eosDeleteLinear(Object)                         eos::FreeLinear(Object)
#define eosDeleteRawLinear(Object)                      eos::FreeRawLinear(Object)

#define eosReallocLinear(Object, Type, Alignment)       eos::ReallocLinear<Type>(Object, sizeof(Type), Alignment)
#define eosReallocRawLinear(Object, Size, Alignment)    eos::ReallocRawLinear(Object, Size, Alignment)


//////////////////////////////////////////////////////////////////////////
#define eosNewStack(Type, Alignment, ...)               new ( eos::g_stackAllocator.Alloc(sizeof(Type), Alignment) ) Type(__VA_ARGS__)
#define eosNewRawStack(Size, Alignment)                 eos::g_stackAllocator.Alloc(Size, Alignment)

#define eosDeleteStack(Object)                          eos::FreeStack(Object)
#define eosDeleteRawStack(Object)                       eos::FreeRawStack(Object)

#define eosReallocStack(Object, Type, Alignment)        eos::ReallocStack<Type>(Object, sizeof(Type), Alignment)
#define eosReallocRawStack(Object, Size, Alignment)     eos::ReallocRawStack(Object, Size, Alignment)
