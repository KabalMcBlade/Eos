#pragma once

#include <atomic>
#include "Core/BasicTypes.h"
#include "MemoryFunctions.h"


EOS_NAMESPACE_BEGIN


template struct std::atomic<uint32>;

class SmartObject
{
public:
	typedef uint32 RefCount;

	static const RefCount kInvalidRefCount = (RefCount)-1;

	SmartObject() : m_referenceCount(0) {}
	virtual ~SmartObject() {}

	EOS_INLINE RefCount RefIncrement()
	{
		return m_referenceCount.fetch_add(1, std::memory_order_relaxed);
	}

	EOS_INLINE RefCount RefDecrement()
	{
		return m_referenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
	}

	EOS_INLINE RefCount GetRefCount() const
	{
		return m_referenceCount.load();
	}

private:
	mutable std::atomic_uint32_t m_referenceCount;
};


//////////////////////////////////////////////////////////////////////////


template <typename T, typename Allocator>
class SmartPointer final
{
private:
	template<typename U, typename Allocator>
	friend class SmartPointer;

public:
	SmartPointer(Allocator* _allocator) : m_allocator(_allocator)
	{
		m_object = eosNew(T, m_allocator);
		RefIncrement(m_object);
	}

	SmartPointer(Allocator* _allocator, T* _value) : m_allocator(_allocator), m_object(_value)
	{
		RefIncrement(m_object);
	}

	template <typename U>
	SmartPointer(SmartPointer<U, Allocator> const & _other) : m_allocator(_other.m_allocator), m_object(_other.m_object)
	{
		RefIncrement(m_object);
	}

	SmartPointer(SmartPointer const & _other) : m_allocator(_other.m_allocator), m_object(_other.m_object)
	{
		RefIncrement(m_object);
	}

	~SmartPointer()
	{
		if (m_object == nullptr)
		{
			return;
		}

		const SmartObject::RefCount refCount = RefDecrement(m_object);

		if (refCount == 0)
		{
			eosDelete(m_object, m_allocator);
		}

		m_object = nullptr;
	}

	SmartPointer & operator=(SmartPointer const &_other)
	{
		SmartPointer(_other).Swap(*this);
		return *this;
	}

	SmartPointer & operator=(T* _other)
	{
		SmartPointer(_other).Swap(*this);
		return *this;
	}

	bool IsValid() const { return m_object != nullptr; }
	uint32 GetRefCount() const { return IsValid() ? GetRefCount(m_object) : 0; }

	T& operator*() const { return *m_object; }
	T* operator->() const { return  m_object; }
	const T* Get() const { return  m_object; }

protected:
	EOS_INLINE SmartObject::RefCount RefIncrement(SmartObject* _object)
	{
		return (_object) ? _object->RefIncrement() : SmartObject::kInvalidRefCount;
	}

	EOS_INLINE SmartObject::RefCount RefDecrement(SmartObject* _object)
	{
		return (_object) ? _object->RefDecrement() : SmartObject::kInvalidRefCount;
	}

	EOS_INLINE SmartObject::RefCount GetRefCount(SmartObject* _object)
	{
		return (_object) ? _object->GetRefCount() : SmartObject::kInvalidRefCount;
	}

	void Swap(SmartPointer &_source)
	{
		std::swap(m_allocator, _source.m_allocator);
		std::swap(m_object, _source.m_object);
	}

	Allocator* m_allocator;
	T * m_object;
};

template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(SmartPointer<T1, typename Allocator> const & _sp1, SmartPointer<T2, typename Allocator> const & _sp2) { return _sp1->Get() == _sp2->Get(); }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(SmartPointer<T1, typename Allocator> const & _sp1, T2* _p2) { return _sp1->Get() == _p2; }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator==(T1* _p1, SmartPointer<T2, typename Allocator> const & _sp2) { return _p1 == _sp2->Get(); }

template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(SmartPointer<T1, typename Allocator> const & _sp1, SmartPointer<T2, typename Allocator> const & _sp2) { return _sp1() != _sp2(); }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(SmartPointer<T1, typename Allocator> const & _sp1, T2* _p2) { return _sp1->Get() != _p2; }
template <class T1, class T2, typename Allocator> EOS_INLINE bool operator!=(T1* _p1, SmartPointer<T2, typename Allocator> const & _sp2) { return _p1 != _sp2->Get(); }

template <class T, typename Allocator>EOS_INLINE bool operator<(SmartPointer<T, typename Allocator> const & _sp1, SmartPointer<T, typename Allocator> const & _sp2) { return _sp1->Get() < _sp2->Get(); }
template <class T, typename Allocator>EOS_INLINE bool operator>(SmartPointer<T, typename Allocator> const & _sp1, SmartPointer<T, typename Allocator> const & _sp2) { return _sp1->Get() > _sp2->Get(); }


EOS_NAMESPACE_END