#include <cstdio>
#include <string>
#include <list>

struct Token {
    enum Type {
        ID, INT_LITERAL, OBR, CBR, END_OF_INPUT, INVALID
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

struct Scanner {
    char const* cursor;
    
    Scanner(char const* str): cursor(str)
    {
    }
    
    Token next()
    {
        char const* tokStart;
        
start:
        tokStart = cursor;
    
        /*!re2c
            re2c:define:YYCTYPE  = "char";
            re2c:define:YYCURSOR = cursor;
            re2c:yyfill:enable   = 0;
        
            WS =            [ \r\n\t\f];
            ID_SYM =        [a-zA-Z!$%&*+-./:<=>?@^_~];
            DIGIT =         [0-9];
            
            "("                         { return Token(Token::OBR, tokStart, cursor); }
            ")"                         { return Token(Token::CBR, tokStart, cursor); }
            ID_SYM (ID_SYM | DIGIT)*    { return Token(Token::ID, tokStart, cursor); }
            DIGIT+                      { return Token(Token::INT_LITERAL, tokStart, cursor); }
            WS                          { goto start; }
            [\000]                      { return Token(Token::END_OF_INPUT); }
            [^]                         { return Token(Token::INVALID, tokStart, cursor); }
        */
    }
};

int main()
{
    Scanner scanner("123 (0456.@$@%789)))");
    
    for(Token tok = scanner.next(); tok.type != Token::END_OF_INPUT; tok = scanner.next())
    {
        if(tok.type == Token::INVALID)
        {
            printf("Unexpected token (%s)\n", tok.text().c_str());
            break;
        }
        else
            printf("(%s)\n", tok.text().c_str());
    }
    
    return 0;
}