/* 
 * File:   AST.hpp
 * Author: stas
 *
 * Created on May 30, 2014, 6:04 PM
 */

#ifndef AST_HPP
#define	AST_HPP

#include <memory>
#include <list>
#include <cstdio>

#include "Token.hpp"

struct ASTNode {
    enum Type {
        LIST, ATOM,
    };
    
    typedef std::shared_ptr<ASTNode> Sptr;
    
    Type type;
    
    ASTNode(Type type_): type(type_)
    {
    }
    
    virtual void print() = 0;
    virtual ~ASTNode() = default;
};

struct List: public ASTNode {
    std::list<ASTNode::Sptr> nodes;
    
    List(): ASTNode(ASTNode::LIST)
    {
    }
    
    void print()
    {
        printf("(");
        
        for(auto node : nodes)
        {
            node->print();
            printf(" ");
        }
        
        printf(")");
    }
};

struct Atom: public ASTNode {
    Token token;
    
    Atom(Token token_): ASTNode(ASTNode::ATOM), token(token_)
    {
    }
    
    void print()
    {
        printf("%s", token.text.c_str());
    }
};

#endif	/* AST_HPP */

