/* 
 * File:   Scanner.hpp
 * Author: stas
 *
 * Created on May 24, 2014, 5:31 PM
 */

#ifndef SCANNER_HPP
#define	SCANNER_HPP

#include <list>

#include "Util.hpp"
#include "Token.hpp"

struct Scanner {
    char const* cursor;
    
    Scanner(char const* str): cursor(str)
    {
    }
    
    Token next();
    
    std::list<Token> scan()
    {
        std::list<Token> tokens;
        
        for(Token tok = next(); tok.type != Token::END_OF_INPUT; tok = next())
        {
            if(tok.type == Token::INVALID)
                error("Unexpected token " + tok.text);
            else
                tokens.push_back(tok);
        }
        
        tokens.push_back(Token(Token::END_OF_INPUT));
        
        return tokens;
    }
};

#endif	/* SCANNER_HPP */

