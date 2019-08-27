#pragma once

#include "CoreDefs.h"

#include "MemoryUtils.h"


EOS_NAMESPACE_BEGIN

template<typename T, class Allocator>
EOS_INLINE void Free(T* _object, Allocator* _allocator)
{
    _object->~T();
    _allocator->Free(_object);
}

template<typename T, class Allocator>
EOS_INLINE T* AllocArray(Allocator* _allocator, eosSize _cnt, const char* _file, eosU32 _line, eosNonPODType)
{
    union
    {
        void* as_void;
        eosSize* as_size;
        T* as_T;
    };

    as_T = AllocArray<T>(_allocator, _cnt, _file, _line, eosPODType());

    const T* const onePastLast = as_T + _cnt;
    while (as_T < onePastLast)
    {
        new (as_T++) T;
    }

    return as_T - _cnt;
}

template<typename T, class Allocator>
EOS_INLINE T* AllocArray(Allocator* _allocator, eosSize _cnt, const char* _file, eosU32 _line, eosPODType)
{
    union
    {
        void* as_void;
        ionSize* as_size;
        T* as_T;
    };

    const eosSize numHeaderElements = sizeof(eosSize) / sizeof(T) + ((eosBool)(sizeof(eosSize) % sizeof(T)) || 0);

    as_void = _allocator->Allocate(sizeof(T) * (_cnt + numHeaderElements), alignof(T), eosSourceInfo(_file, _line));

    // store number of elements at the back of the first element of the array.
    as_T += numHeaderElements;
    *(as_size - 1) = _cnt;

    return as_T;
}

template <typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator* _allocator)
{
    FreeArray(_ptr, _allocator, eosIntToType<eosIsPOD<T>::value>());
}

template<typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator* _allocator, eosNonPODType)
{
    union
    {
        eosSize* as_size;
        T* as_T;
    };

    as_T = _ptr;
    const eosSize _cnt = as_size[-1];

    for (eosSize i = _cnt; i > 0; --i)
    {
        as_T[i - 1].~T();
    }

    const eosSize numHeaderElements = sizeof(eosSize) / sizeof(T) + ((eosBool)(sizeof(eosSize) % sizeof(T)) || 0);

    _allocator->Free(as_T - numHeaderElements);
}

template<typename T, class Allocator>
EOS_INLINE void FreeArray(T* _ptr, Allocator& _allocator, eosPODType)
{
    union
    {
        eosSize* as_size;
        T* as_T;
    };

    as_T = _ptr;
    const eosSize numHeaderElements = sizeof(eosSize) / sizeof(T) + ((eosBool)(sizeof(eosSize) % sizeof(T)) || 0);

    _allocator->Free(as_T - numHeaderElements);
}


EOS_NAMESPACE_END


// Put outside of scope using namespace scope when required in order to allow to use define without namespace scope
//////////////////////////////////////////////////////////////////////////

#define eosNewAligned(Type, Allocator, Alignment, ...)  new ((Allocator)->Allocate(sizeof(Type), Alignment, EOS_MEMORY_SOURCE_ALLOCATION_INFO)) Type(__VA_ARGS__)
#define eosNew(Type, Allocator, ...)                    eosNewAligned(Type, (Allocator), alignof(Type), __VA_ARGS__)
#define eosDelete(Object, Allocator)                    eos::Free((Object), (Allocator))

#define eosNewDynamicArray(Type, Count, Allocator)      eos::AllocArray<Type>((Allocator), Count, __FILE__, __LINE__, eos::eosIntToType<eos::eosIsPOD<Type>::value>())
#define eosNewArray(Type, Allocator)                    eos::AllocArray<eos::eosTypeAndCount<Type>::type>((Allocator), eos::eosTypeAndCount<Type>::count, __FILE__, __LINE__, eos::eosIntToType<eos::eosIsPOD<eos::eosTypeAndCount<Type>::type>::value>())
#define eosDeleteArray(ObjectArray, Allocator)          eos::FreeArray((ObjectArray), (Allocator))
