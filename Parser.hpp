/* 
 * File:   Parser.hpp
 * Author: stas
 *
 * Created on May 30, 2014, 6:03 PM
 */

#ifndef PARSER_HPP
#define	PARSER_HPP

#include <list>

#include "Util.hpp"
#include "Token.hpp"
#include "AST.hpp"

/*
 * expr = (list | atom)
 * list = '(' expr* ')'
 * atom = name | literal
*/

struct Parser {
    typedef std::list<Token>::const_iterator TokenIter;
    
    TokenIter cursor;
    
    Parser(TokenIter start): cursor(start)
    {
    }
    
    // **********
    // * Errors *
    // **********
    
    void expectedTokenError(Token::Type tok)
    {
        throw CompilationError("Expected '" + Token::typeName(tok) + "'");
    }
    
    void unexpectedTokenError(Token const& tok)
    {
        if(tok.type == Token::INVALID)
            throw CompilationError("Unexpected '" + tok.text + "'");
        else
            throw CompilationError("Unexpected '" + Token::typeName(tok.type) + "'");
    }
    
    // ********
    // * Util *
    // ********
    
    bool end()
    {
        return cursor->type == Token::END_OF_INPUT;
    }
    
    Token::Type peek()
    {
        return cursor->type;
    }
    
    void readToken(Token::Type tok)
    {
        if(peek() != tok)
            expectedTokenError(tok);
        ++cursor;
    }
    
    Token::Type readToken()
    {
        return (cursor++)->type;
    }
    
    // *********
    // * Rules *
    // *********
    
    ASTNode::Sptr readList()
    {
        std::shared_ptr<List> lst(new List());
        
        readToken(Token::OBR);
        
        while(peek() != Token::CBR)
        {
            lst->nodes.push_back(readExpr());
        }
        
        readToken(Token::CBR);
        
        return lst;
    }
    
    static bool isAtom(Token::Type tokType)
    {
        return tokType != Token::CBR &&
                tokType != Token::OBR &&
                tokType != Token::END_OF_INPUT &&
                tokType != Token::INVALID;
    }
    
    ASTNode::Sptr readAtom()
    {
        Token::Type tokType = peek();
        
        if(isAtom(tokType))
            return ASTNode::Sptr(new Atom(*(cursor++)));
        else
            unexpectedTokenError(*cursor);
    }
    
    ASTNode::Sptr readExpr()
    {
        switch(peek())
        {
            case Token::OBR:
                return readList();
            default:
                return readAtom();
        }
    }
};

#endif	/* PARSER_HPP */

