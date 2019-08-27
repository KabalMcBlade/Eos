#pragma once

#include "CoreDefs.h"

EOS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
class eosMutex
{
public:
    EOS_INLINE void Enter()
    {
        m_mutex.lock();
    }

    EOS_INLINE void Leave()
    {
        m_mutex.unlock();
    }

private:
    std::mutex m_mutex;
};


////////////////////////////////////////////////////////////////////////////////
class eosSharedMutex
{
public:
    EOS_INLINE void Enter()
    {
        m_mutex.lock();
    }

    EOS_INLINE void Leave()
    {
        m_mutex.unlock();
    }

private:
    std::shared_mutex m_mutex;
};


////////////////////////////////////////////////////////////////////////////////
class eosRecursiveMutex
{
public:
    EOS_INLINE void Enter()
    {
        m_mutex.lock();
    }

    EOS_INLINE void Leave()
    {
        m_mutex.unlock();
    }

private:
    std::recursive_mutex m_mutex;
};

////////////////////////////////////////////////////////////////////////////////
class eosSingleThread
{
public:
    EOS_INLINE void Enter() {};
    EOS_INLINE void Leave() {};
};

////////////////////////////////////////////////////////////////////////////////
template<typename SynchronizateosMutex>
class eosMultiThread
{
public:
    EOS_INLINE void Enter() 
    {
        m_syncMutex.Enter(); 
    }

    EOS_INLINE void Leave()
    {
        m_syncMutex.Leave();
    }

private:
    SynchronizateosMutex m_syncMutex;
};

////////////////////////////////////////////////////////////////////////////////

using eosDefaultMultiThreadPolicy = eosMultiThread<eosMutex>;
using eosDefaultSingleThreadPolicy = eosSingleThread;

EOS_NAMESPACE_END
