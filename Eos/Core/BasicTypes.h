#pragma once

#include <cstddef>	// for size_t
#include <cstdint>	// for everything else

#include "BasicDefines.h"

EOS_NAMESPACE_BEGIN

typedef std::int8_t     int8;
typedef std::int16_t    int16;
typedef std::int32_t    int32;
typedef std::int64_t    int64;

typedef std::uint8_t    uint8;
typedef std::uint16_t   uint16;
typedef std::uint32_t   uint32;
typedef std::uint64_t   uint64;

typedef std::size_t     size;

typedef std::intptr_t   intPtr;
typedef std::uintptr_t  uintPtr;

EOS_NAMESPACE_END