#pragma once

#include "StlAllocator.h"


// are only by STL, so I know the library is the same for me
#pragma warning(disable:4251)   


EOS_NAMESPACE_BEGIN


template<typename T, typename Allocator> using eosVector = std::vector<T, eosStlAllocator<T, Allocator> >;
template<typename T, typename Allocator> using eosList = std::list<T, eosStlAllocator<T, Allocator> >;
template<typename T, typename Allocator> using eosStack = std::stack<T, eosStlAllocator<T, Allocator> >;
template<typename T, typename Allocator> using eosDeque = std::deque<T, eosStlAllocator<T, Allocator> >;
template<typename T, typename Allocator> using eosQueue = std::queue<T, eosDeque<T, Allocator> >;

template<typename K, typename V, typename Allocator> using eosMapAllocator = eosStlAllocator<std::pair<const K, V>, Allocator>;
template<typename K, typename V, typename Allocator, typename Compare = std::less<K>> using eosMap = std::map<K, V, Compare, eosMapAllocator<K, V, Allocator>>;
template<typename K, typename V, typename Allocator, typename Compare = std::less<K>> using eosUnorderedMap = std::unordered_map<K, V, Compare, eosMapAllocator<K, V, Allocator>>;

template<class Allocator> using eosString = std::basic_string<char, std::char_traits<char>, eosStlAllocator<char, Allocator>>;
template<class Allocator> using eosWString = std::basic_string<wchar_t, std::char_traits<wchar_t>, eosStlAllocator<wchar_t, Allocator>>;

template<class Allocator> using eosOStream = std::basic_ostringstream<char, std::char_traits<char>, eosStlAllocator<char, Allocator> >;
template<class Allocator> using eosStringStream = std::basic_stringstream<char, std::char_traits<char>, eosStlAllocator<char, Allocator> >;
template<class Allocator> using eosIStringStream = std::basic_istringstream<char, std::char_traits<char>, eosStlAllocator<char, Allocator> >;

template<class Allocator> using eosWOStream = std::basic_ostringstream<wchar_t, std::char_traits<wchar_t>, eosStlAllocator<wchar_t, Allocator> >;
template<class Allocator> using eosWStringStream = std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, eosStlAllocator<wchar_t, Allocator> >;
template<class Allocator> using eosWIStringStream = std::basic_istringstream<wchar_t, std::char_traits<wchar_t>, eosStlAllocator<wchar_t, Allocator> >;


EOS_NAMESPACE_END