#pragma once

#include "CoreDefs.h"


EOS_NAMESPACE_BEGIN

class eosNonCopyable
{
public:
    eosNonCopyable(const eosNonCopyable &) = delete;
    eosNonCopyable & operator = (const eosNonCopyable &) = delete;

protected:
    eosNonCopyable() = default;
    ~eosNonCopyable() = default; /// Protected non-virtual destructor
};

class eosNonMoveable
{
public:
    eosNonMoveable(const eosNonMoveable &&) = delete;
    eosNonMoveable & operator = (const eosNonMoveable &&) = delete;

protected:
    eosNonMoveable() = default;
    ~eosNonMoveable() = default; /// Protected non-virtual destructor
};

class eosNonCopyMove
{
public:
    eosNonCopyMove(const eosNonCopyMove &) = delete;
    eosNonCopyMove & operator = (const eosNonCopyMove &) = delete;
    eosNonCopyMove(const eosNonCopyMove &&) = delete;
    eosNonCopyMove & operator = (const eosNonCopyMove &&) = delete;

protected:
    eosNonCopyMove() = default;
    ~eosNonCopyMove() = default; /// Protected non-virtual destructor
};

EOS_NAMESPACE_END