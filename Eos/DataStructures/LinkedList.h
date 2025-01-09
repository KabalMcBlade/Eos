// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\DataStructures\LinkedList.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "../Core/NoCopyable.h"


EOS_NAMESPACE_BEGIN

template <typename T>
class LinkedList : public NoCopyableMoveable
{
public:
	struct Node
	{
		T m_data;
		Node* m_next;
	};

	LinkedList() {}
	~LinkedList() {}

	void Push(Node* _add)
	{
		if (m_head != nullptr)
		{
			_add->m_next = m_head;
		}
		else
		{
			_add->m_next = nullptr;
		}
		m_head = _add;
	}

	void Insert(Node* _prev, Node* _add)
	{
		if (_prev == nullptr)
		{
			Push(_add);
		}
		else
		{
			if (_prev->m_next == nullptr)
			{
				_prev->m_next = _add;
				_add->m_next = nullptr;
			}
			else 
			{
				_add->m_next = _prev->m_next;
				_prev->m_next = _add;
			}
		}
	}

	void Remove(Node* _prev, Node* _remove)
	{
		if (_prev == nullptr)
		{
			if (_remove->m_next == nullptr)
			{
				m_head = nullptr;
			}
			else
			{
				m_head = _remove->m_next;
			}
		}
		else 
		{
			_prev->m_next = _remove->m_next;
		}
	}

	void SetHead(Node* _head)
	{
		m_head = _head;
	}

	Node* GetHead()
	{
		return m_head;
	}

	const Node* GetHead() const
	{
		return m_head;
	}

private:
	Node* m_head;
};

EOS_NAMESPACE_END