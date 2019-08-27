#pragma once

#include "CoreDefs.h"
#include "NonCopyable.h"

#include "VirtualMemory.h"

EOS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
template<eosSize Size>
class eosFixedStackArea : eosNonCopyMove
{
public:
    eosFixedStackArea()
    {
    }

    EOS_INLINE void* GetStart() const { return (void*)(m_memory); }
    EOS_INLINE void* GetEnd() const { return (void*)(m_memory + Size); }

private:
    eosU8 m_memory[Size];
};

////////////////////////////////////////////////////////////////////////////////
class eosDynamicStackArea : eosNonCopyMove
{
public:
    eosDynamicStackArea(eosSize _size)
    {
        m_start = _alloca(_size);
        m_end = reinterpret_cast<void*>(reinterpret_cast<eosUPtr>(m_start) + _size);
    }

    ~eosDynamicStackArea()
    {
        _freea(m_start);
    }

    EOS_INLINE void* GetStart() const { return m_start; }
    EOS_INLINE void* GetEnd() const { return m_end; }

private:
    void* m_start;
    void* m_end;
};

////////////////////////////////////////////////////////////////////////////////
class eosHeapArea : eosNonCopyMove
{
public:
    explicit eosHeapArea(eosSize _size)
    {
        m_start = malloc(_size);
        m_end = reinterpret_cast<void*>(reinterpret_cast<eosUPtr>(m_start) + _size);
    }

    ~eosHeapArea()
    {
        free(m_start);
    }

    EOS_INLINE void* GetStart() const { return m_start; }
    EOS_INLINE void* GetEnd() const { return m_end; }

private:
    void* m_start;
    void* m_end;
};

////////////////////////////////////////////////////////////////////////////////
class eosVirtualHeapArea : eosNonCopyMove
{
public:
    explicit eosVirtualHeapArea(eosSize _size)
    {
        m_start = eosVirtualMemory::ReserveAddressSpace(_size);
        m_end = reinterpret_cast<void*>(reinterpret_cast<eosUPtr>(m_start) + _size);
    }

    ~eosVirtualHeapArea()
    {
        eosVirtualMemory::ReleaseAddressSpace(m_start);
    }

    EOS_INLINE void* GetStart() const { return m_start; }
    EOS_INLINE void* GetEnd() const { return m_end; }

private:
    void* m_start;
    void* m_end;
};

////////////////////////////////////////////////////////////////////////////////
class eosDynamicHeapArea : eosNonCopyMove
{
public:
    eosDynamicHeapArea(eosSize _startSize, eosSize _maxSize) : m_startSize(_startSize), m_maxSize(_maxSize)
    {
    }

    EOS_INLINE eosSize GetStartSize() const { return m_startSize; }
    EOS_INLINE eosSize GetMaxSize() const { return m_maxSize; }

private:
    eosSize m_startSize;
    eosSize m_maxSize;
};

EOS_NAMESPACE_END
