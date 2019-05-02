#pragma once

#include "StlAllocators.h"


// are only by STL, so I know the library is the same for me
#pragma warning(disable:4251)   


EOS_NAMESPACE_BEGIN


using eosString = std::basic_string<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >;
using eosOsStream = std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >;
using eosStringStream = std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >;
using eosIStringStream = std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >;

using eosStringFast = std::basic_string<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >;
using eosOsStreamFast = std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >;
using eosStringStreamFast = std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >;
using eosIStringStreamFast = std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >;

using eosStringStack = std::basic_string<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >;
using eosOsStreamStack = std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >;
using eosStringStreamStack = std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >;
using eosIStringStreamStack = std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >;

template<typename T> using eosGenericString = std::basic_string<T, std::char_traits<T>, StlAllocator<T, HeapAllocPolicy<T>> >;
template<typename T> using eosGenericOsStream = std::basic_ostringstream<T, std::char_traits<T>, StlAllocator<T, HeapAllocPolicy<T>> >;
template<typename T> using eosGenericStringStream = std::basic_stringstream<T, std::char_traits<T>, StlAllocator<T, HeapAllocPolicy<T>> >;
template<typename T> using eosGenericIStringStream = std::basic_istringstream<T, std::char_traits<T>, StlAllocator<T, HeapAllocPolicy<T>> >;

template<typename T> using eosGenericStringFast = std::basic_string<T, std::char_traits<T>, StlAllocator<T, LinearAllocPolicy<T>> >;
template<typename T> using eosGenericOsStreamFast = std::basic_ostringstream<T, std::char_traits<T>, StlAllocator<T, LinearAllocPolicy<T>> >;
template<typename T> using eosGenericStringStreamFast = std::basic_stringstream<T, std::char_traits<T>, StlAllocator<T, LinearAllocPolicy<T>> >;
template<typename T> using eosGenericIStringStreamFast = std::basic_istringstream<T, std::char_traits<T>, StlAllocator<T, LinearAllocPolicy<T>> >;

template<typename T> using eosGenericStringStack = std::basic_string<T, std::char_traits<T>, StlAllocator<T, StackAllocPolicy<T>> >;
template<typename T> using eosGenericOsStreamStack = std::basic_ostringstream<T, std::char_traits<T>, StlAllocator<T, StackAllocPolicy<T>> >;
template<typename T> using eosGenericStringStreamStack = std::basic_stringstream<T, std::char_traits<T>, StlAllocator<T, StackAllocPolicy<T>> >;
template<typename T> using eosGenericIStringStreamStack = std::basic_istringstream<T, std::char_traits<T>, StlAllocator<T, StackAllocPolicy<T>> >;


template<typename T> using eosVector = std::vector<T, StlAllocator<T, HeapAllocPolicy<T> > >;
template<typename T> using eosStack = std::stack<T, StlAllocator<T, HeapAllocPolicy<T> > >;
template<typename T> using eosDeque = std::deque<T, StlAllocator<T, HeapAllocPolicy<T> > >;
template<typename T> using eosQueue = std::queue<T, eosDeque<T> >;
template<typename T> using eosList = std::list<T, StlAllocator<T, HeapAllocPolicy<T> > >;

template<typename T> using eosVectorFast = std::vector<T, StlAllocator<T, LinearAllocPolicy<T> > >;
template<typename T> using eosStackFast = std::stack<T, StlAllocator<T, LinearAllocPolicy<T> > >;
template<typename T> using eosDequeFast = std::deque<T, StlAllocator<T, LinearAllocPolicy<T> > >;
template<typename T> using eosQueueFast = std::queue<T, eosDequeFast<T> >;
template<typename T> using eosListFast = std::list<T, StlAllocator<T, LinearAllocPolicy<T> > >;

template<typename T> using eosVectorStack = std::vector<T, StlAllocator<T, StackAllocPolicy<T> > >;
template<typename T> using eosStackStack = std::stack<T, StlAllocator<T, StackAllocPolicy<T> > >;
template<typename T> using eosDequeStack = std::deque<T, StlAllocator<T, StackAllocPolicy<T> > >;
template<typename T> using eosQueueStack = std::queue<T, eosDequeStack<T> >;
template<typename T> using eosListStack = std::list<T, StlAllocator<T, StackAllocPolicy<T> > >;

template<typename T, typename U> using eosMap = std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, HeapAllocPolicy<std::pair<T, U>> > >;
template<typename T, typename U> using eosUnorderedMap = std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, HeapAllocPolicy<std::pair<T, U>> > >;
template<typename P0, typename P1, typename T> using eosMapExplicit = std::map<std::pair<P0, P1>, T, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, HeapAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;
template<typename P0, typename P1, typename T> using eosUnorderedMapExplicit = std::unordered_map<std::pair<P0, P1>, T, hash_pair<P0, P1>, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, HeapAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;

template<typename T, typename U> using eosMapFast = std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, LinearAllocPolicy<std::pair<T, U>> > >;
template<typename T, typename U> using eosUnorderedMapFast = std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, LinearAllocPolicy<std::pair<T, U>> > >;
template<typename P0, typename P1, typename T> using eosMapExplicitFast = std::map<std::pair<P0, P1>, T, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, LinearAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;
template<typename P0, typename P1, typename T> using eosUnorderedMapExplicitFast = std::unordered_map<std::pair<P0, P1>, T, hash_pair<P0, P1>, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, LinearAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;

template<typename T, typename U> using eosMapStack = std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, StackAllocPolicy<std::pair<T, U>> > >;
template<typename T, typename U> using eosUnorderedMapStack = std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, StackAllocPolicy<std::pair<T, U>> > >;
template<typename P0, typename P1, typename T> using eosMapExplicitStack = std::map<std::pair<P0, P1>, T, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, StackAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;
template<typename P0, typename P1, typename T> using eosUnorderedMapExplicitStack = std::unordered_map<std::pair<P0, P1>, T, hash_pair<P0, P1>, std::less<std::pair<P0, P1>>, StlAllocator<std::pair<std::pair<P0, P1>, T>, StackAllocPolicy<std::pair<std::pair<P0, P1>, T>> > >;



EOS_NAMESPACE_END