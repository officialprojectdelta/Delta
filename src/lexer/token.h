#pragma once

#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include "error/error.h"

enum class TokenType
{
    OBRACKET,
    CBRACKET,
    OPAREN,
    CPAREN,
    SEMI,
    COMMA,
    NOT,
    BITCOMPL,
    DASH,
    INC,
    DEC,
    ADD,
    MUL,
    DIV,
    AND,
    OR,
    EQ,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
    TINT,
    RET,
    IF,
    ELSE,
    FOR,
    WHILE,
    DO,
    BREAK,
    CONTINUE,
    ASSIGN, 
    INTV,
    IDENT,
    NULLTOK
};

// A lexer token containing a type and a value
struct Token
{
    TokenType type;
    std::string value;

    Token()
        : type(TokenType::NULLTOK), value(std::string())
    {

    }

    Token(TokenType type)
        : type(type), value(std::string())
    {

    }

    Token(TokenType type, const std::string& value)
        : type(type), value(value)
    {

    }
};

// The tokenizer class to manage the token vector, in the parser
class Tokenizer
{
private:
    std::vector<Token> tokens;
    size_t pos = 0;
public:
    Tokenizer(std::vector<Token>&& tokens)
        : tokens(std::move(tokens)), pos(0)
    {   

    }

    Token& operator[](size_t idx) { return tokens.at(idx); }
    
    Token cur() { return tokens.at(pos); };
    Token cur(size_t idx) { return tokens.at(pos + idx); }
    Token inc() { pos++; return tokens.at(pos - 1); }
    Token next() { if (pos + 1 >= tokens.size()) return Token(TokenType::NULLTOK); return tokens.at(pos + 1); }
    size_t size() { return tokens.size(); }
    size_t getPos() { return pos; }
    void check(const std::string& str) { if (pos >= tokens.size()) throw compiler_error(str); }
};

std::ostream& operator<<(std::ostream& os, const Token& t);