// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\MemoryAreaPolicy.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <memory>
#include "Core/NoCopyable.h"


EOS_NAMESPACE_BEGIN



template<size Size>
class StackArea : public NoCopyableMoveable
{
public:
	StackArea()
	{
	}

	EOS_INLINE void* GetStart() const { return (void*)(m_memory); }
	EOS_INLINE void* GetEnd() const { return (void*)(m_memory + Size); }

private:
	int8 m_memory[Size];
};


template<size Size>
class HeapArea : public NoCopyableMoveable
{
public:
	HeapArea()
	{
		m_start = malloc(Size);
		m_end = reinterpret_cast<void*>(reinterpret_cast<uintPtr>(m_start) + Size);
	}

	~HeapArea()
	{
		free(m_start);
	}

	EOS_INLINE void* GetStart() const { return m_start; }
	EOS_INLINE void* GetEnd() const { return m_end; }

private:
	void* m_start;
	void* m_end;
};

// R is for "runtime" to distinguished against the normal template version
class HeapAreaR : public NoCopyableMoveable
{
public:
	HeapAreaR(size _size)
	{
		m_start = malloc(_size);
		m_end = reinterpret_cast<void*>(reinterpret_cast<uintPtr>(m_start) + _size);
	}

	~HeapAreaR()
	{
		free(m_start);
	}

	EOS_INLINE void* GetStart() const { return m_start; }
	EOS_INLINE void* GetEnd() const { return m_end; }

private:
	void* m_start;
	void* m_end;
};



EOS_NAMESPACE_END