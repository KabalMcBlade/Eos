// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\DataStructures\StackLinkedList.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "../Core/NoCopyable.h"


EOS_NAMESPACE_BEGIN

template <typename T>
class StackLinkedList : public NoCopyableMoveable
{
public:
	struct Node
	{
		T m_data;
		Node* m_next;
	};

	StackLinkedList() {}
	~StackLinkedList() {}

	void Push(Node* _add)
	{
		_add->m_next = m_head;
		m_head = _add;
	}

	Node* Pop()
	{
		Node* top = m_head;
		m_head = m_head->m_next;
		return top;
	}

	const Node* Peak() const
	{
		return m_head;
	}

private:
	Node* m_head;
};

EOS_NAMESPACE_END