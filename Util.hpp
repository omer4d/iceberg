#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <cstdio>

void error(std::string const& msg)
{
    printf("%s", msg.c_str());
    char* ptr = nullptr;
    *ptr = 0;
}

#endif