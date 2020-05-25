#pragma once

#include "BasicTypes.h"


EOS_NAMESPACE_BEGIN


//
class NoCopyable
{
public:
	NoCopyable(const NoCopyable &) = delete;
	NoCopyable & operator = (const NoCopyable &) = delete;

protected:
	NoCopyable() = default;
	~NoCopyable() = default;
};

//
class NoMoveable
{
public:
	NoMoveable(const NoMoveable &&) = delete;
	NoMoveable & operator = (const NoMoveable &&) = delete;

protected:
	NoMoveable() = default;
	~NoMoveable() = default;
};

//
class NoCopyableMoveable
{
public:
	NoCopyableMoveable(const NoCopyableMoveable &) = delete;
	NoCopyableMoveable & operator = (const NoCopyableMoveable &) = delete;
	NoCopyableMoveable(const NoCopyableMoveable &&) = delete;
	NoCopyableMoveable & operator = (const NoCopyableMoveable &&) = delete;

protected:
	NoCopyableMoveable() = default;
	~NoCopyableMoveable() = default;
};


EOS_NAMESPACE_END