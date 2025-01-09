// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\Core\Assertions.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <cstdio>

#include "BasicDefines.h"


#ifdef _DEBUG

#define eosAssert( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
    }
#define eosAssertVoid( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return; \
    }
#define eosAssertValue( condition, return_value, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return return_value; \
    }

#else

#define eosAssert( condition, format, ... )
#define eosAssertVoid( condition, format, ... )
#define eosAssertValue( condition, return_value, format, ... )

#endif 


#define eosAssertReturnVoid( condition, format, ... )					eosAssertVoid( condition, format, __VA_ARGS__ )
#define eosAssertReturnValue( condition, return_value, format, ...  )	eosAssertValue( condition, return_value, format, __VA_ARGS__ )