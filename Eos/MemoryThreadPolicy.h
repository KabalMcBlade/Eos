// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryThreadPolicy.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <thread>
#include <mutex>
#include <shared_mutex>

#include "Core/BasicTypes.h"


EOS_NAMESPACE_BEGIN


class Mutex
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


class SharedMutex
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


class RecursiveMutex
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


class SingleThread
{
public:
	EOS_INLINE void Enter() {};
	EOS_INLINE void Leave() {};
};


template<typename SynchronizateosMutex>
class MultiThread
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


using MultiThreadPolicy = MultiThread<Mutex>;
using SingleThreadPolicy = SingleThread;


EOS_NAMESPACE_END