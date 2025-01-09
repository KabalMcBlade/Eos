// Copyright (c) 2018-2025 Michele Condo'
// File: C:\Projects\Eos\Eos\StlAllocatorsTypes.h
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <set>

#include "StlAllocator.h"


// are only by STL, so I know the library is the same for me
#pragma warning(disable:4251)   


EOS_NAMESPACE_BEGIN


template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using Vector = std::vector<T, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using List = std::list<T, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using Stack = std::stack<T, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using Deque = std::deque<T, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using Queue = std::queue<T, Deque<T, Allocator, _AllocatorCallback, Align> >;

template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(std::pair<const K, V>)> using MapAllocator = StlAllocator<std::pair<const K, V>, Allocator, _AllocatorCallback, Align>;
template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>, size Align = __alignof(std::pair<const K, V>)> using Map = std::map<K, V, Compare, MapAllocator<K, V, Allocator, _AllocatorCallback, Align>>;
template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>, size Align = __alignof(std::pair<const K, V>)> using UnorderedMap = std::unordered_map<K, V, Compare, MapAllocator<K, V, Allocator, _AllocatorCallback, Align>>;

template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using String = std::basic_string<char, std::char_traits<char>, StlAllocator<char, Allocator, _AllocatorCallback>>;
template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Allocator, _AllocatorCallback>>;

template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using OStream = std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, Allocator, _AllocatorCallback> >;
template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using StringStream = std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, Allocator, _AllocatorCallback> >;
template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using IStringStream = std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, Allocator, _AllocatorCallback> >;

template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using WOStream = std::basic_ostringstream<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Allocator, _AllocatorCallback> >;
template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using WStringStream = std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Allocator, _AllocatorCallback> >;
template<class Allocator, Allocator*(*_AllocatorCallback)(void)> using WIStringStream = std::basic_istringstream<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Allocator, _AllocatorCallback> >;

template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<T>, size Align = __alignof(T)> using Set = std::set<T, Compare, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Hasher = std::hash<T>, typename KeyEquality = std::equal_to<T>, size Align = __alignof(T)> using UnorderedSet = std::unordered_set<T, Hasher, KeyEquality, StlAllocator<T, Allocator, _AllocatorCallback, Align> >;


EOS_NAMESPACE_END