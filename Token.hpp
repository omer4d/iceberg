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
    std::string text;
    
    Token(Type type_): type(type_)
    {
        
    }
    
    Token(Type type_, char const* start, char const* end): type(type_), text(start, end)
    {
    }
};


#endif	/* TOKEN_HPP */

