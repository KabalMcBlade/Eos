#pragma once

#include "StlAllocators.h"


// are only by STL, so I know the library is the same for me
#pragma warning(disable:4251)   


EOS_NAMESPACE_BEGIN

#define eosString                           std::basic_string<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >
#define eosStringFast                       std::basic_string<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >
#define eosStringStack                      std::basic_string<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >

#define eosTString                           std::basic_string<TCHAR, std::char_traits<TCHAR>, StlAllocator<TCHAR, HeapAllocPolicy<TCHAR>> >
#define eosTStringFast                       std::basic_string<TCHAR, std::char_traits<TCHAR>, StlAllocator<TCHAR, LinearAllocPolicy<TCHAR>> >
#define eosTStringStack                      std::basic_string<TCHAR, std::char_traits<TCHAR>, StlAllocator<TCHAR, StackAllocPolicy<TCHAR>> >

#define eosVector(T)                        std::vector<T, StlAllocator<T, HeapAllocPolicy<T> > >
#define eosVectorFast(T)                    std::vector<T, StlAllocator<T, LinearAllocPolicy<T> > >
#define eosVectorStack(T)                   std::vector<T, StlAllocator<T, StackAllocPolicy<T> > >

#define eosStack(T)                         std::stack<T, StlAllocator<T, HeapAllocPolicy<T> > >
#define eosStackFast(T)                     std::stack<T, StlAllocator<T, LinearAllocPolicy<T> > >
#define eosStackStack(T)                    std::stack<T, StlAllocator<T, StackAllocPolicy<T> > >

#define eosMap(T, U)                        std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, HeapAllocPolicy<std::pair<T, U>> > >
#define eosMapFast(T, U)                    std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, LinearAllocPolicy<std::pair<T, U>> > >
#define eosMapStack(T, U)                   std::map<T, U, std::less<T>, StlAllocator<std::pair<T, U>, StackAllocPolicy<std::pair<T, U>> > >

#define eosUnorderedMap(T, U)               std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, HeapAllocPolicy<std::pair<T, U>> > >
#define eosUnorderedMapFast(T, U)           std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, LinearAllocPolicy<std::pair<T, U>> > >
#define eosUnorderedMapStack(T, U)          std::unordered_map<T, U, std::hash<T>, std::less<T>, StlAllocator<std::pair<T, U>, StackAllocPolicy<std::pair<T, U>> > >

#define eosDeque(T)                         std::deque<T, StlAllocator<T, HeapAllocPolicy<T> > >
#define eosDequeFast(T)                     std::deque<T, StlAllocator<T, LinearAllocPolicy<T> > >
#define eosDequeStack(T)                    std::deque<T, StlAllocator<T, StackAllocPolicy<T> > >

#define eosQueue(T)                         std::queue<T, eosDeque(T) >
#define eosQueueFast(T)                     std::queue<T, eosDequeFast(T) >
#define eosQueueStack(T)                    std::queue<T, eosDequeStack(T) >

#define eosList(T)                          std::list<T, StlAllocator<T, HeapAllocPolicy<T> > >
#define eosListFast(T)                      std::list<T, StlAllocator<T, LinearAllocPolicy<T> > >
#define eosListStack(T)                     std::list<T, StlAllocator<T, StackAllocPolicy<T> > >

#define eosOsStream                         std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >
#define eosOsStreamFast                     std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >
#define eosOsStreamStack                    std::basic_ostringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >

#define eosStringStream                     std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >
#define eosStringStreamFast                 std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >
#define eosStringStreamStack                std::basic_stringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >

#define eosIStringStream                    std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, HeapAllocPolicy<char>> >
#define eosIStringStreamFast                std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, LinearAllocPolicy<char>> >
#define eosIStringStreamStack               std::basic_istringstream<char, std::char_traits<char>, StlAllocator<char, StackAllocPolicy<char>> >

EOS_NAMESPACE_END