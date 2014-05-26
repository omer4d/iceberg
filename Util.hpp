#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <cstdio>
#include <string>
#include <cstring>

inline void die(std::string const& msg)
{
    printf("%s", msg.c_str());
    char* ptr = nullptr;
    *ptr = 0;
}

template <typename T>
T const& min(T const& a, T const& b)
{
    return a < b ? a : b;
}

template<int N>
void copyToBuff(char buff[N], std::string const& str)
{
    strncpy(buff, str.c_str(), std::min(N, (int)str.length() + 1));
}

#endif