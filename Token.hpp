/* 
 * File:   Token.hpp
 * Author: stas
 *
 * Created on May 24, 2014, 5:27 PM
 */

#ifndef TOKEN_HPP
#define	TOKEN_HPP

#include <string>

struct Token {
    enum Type {
        NAME, INT_LITERAL, OBR, CBR, END_OF_INPUT, INVALID
    };
    
    Type type;
    char const* start;
    char const* end;
    
    Token(Type type_): type(type_), start(nullptr), end(nullptr)
    {
        
    }
    
    Token(Type type_, char const* start_, char const* end_): type(type_), start(start_), end(end_)
    {
    }
    
    std::string text()
    {
        return std::string(start, end);
    }
};


#endif	/* TOKEN_HPP */

