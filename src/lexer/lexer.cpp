#include "lexer.h"

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <string.h>

#include "error/error.h"
#include "util.h"

// Constants used to see if a string fits within a group
#define DIGITS "0123456789"
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define ALPHANUMERIC "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"

// Lexer character map (so that it is easier to tokenize characters)
std::unordered_map<std::string, Token> map({
    {"{", Token(TokenType::OBRACKET)}, 
    {"}", Token(TokenType::CBRACKET)}, 
    {"(", Token(TokenType::OPAREN)}, 
    {")", Token(TokenType::CPAREN)}, 
    {";", Token(TokenType::SEMI)}, 
    {",", Token(TokenType::COMMA)}, 
    {"!", Token(TokenType::NOT)}, 
    {"~", Token(TokenType::BITCOMPL)}, 
    {"-", Token(TokenType::DASH)}, 
    {"+", Token(TokenType::ADD)}, 
    {"++", Token(TokenType::INC)},
    {"--", Token(TokenType::DEC)}, 
    {"*", Token(TokenType::MUL)}, 
    {"/", Token(TokenType::DIV)}, 
    {"&&", Token(TokenType::AND)}, 
    {"||", Token(TokenType::OR)}, 
    {"==", Token(TokenType::EQ)}, 
    {"!=", Token(TokenType::NOTEQ)}, 
    {"<", Token(TokenType::LESS)}, 
    {"<=", Token(TokenType::LESSEQ)}, 
    {">", Token(TokenType::GREATER)}, 
    {">=", Token(TokenType::GREATEREQ)}, 
    {"int", Token(TokenType::TINT)},
    {"float", Token(TokenType::TFLOAT)}, 
    {"return", Token(TokenType::RET)}, 
    {"if", Token(TokenType::IF)}, 
    {"else", Token(TokenType::ELSE)}, 
    {"for", Token(TokenType::FOR)}, 
    {"while", Token(TokenType::WHILE)}, 
    {"do", Token(TokenType::DO)}, 
    {"break", Token(TokenType::BREAK)}, 
    {"continue", Token(TokenType::CONTINUE)}, 
    {"=", Token(TokenType::ASSIGN)}, 
    {"[integer]", Token(TokenType::INTV)}, 
    {"[float]", Token(TokenType::FLOATV)},
    {"[alphan]", Token(TokenType::IDENT)}
});

// The lexer function, which takes in a string (the data to be scanned)
Tokenizer scan(std::string data)
{
    // A array of tokens, the output
    std::vector<Token> tokens;

    // The lexer loop
    for (size_t i = 0; i < data.size(); )
    {
        // Skip on whitespace
        if (data[i] == ' ' || data[i] == '\n' || data[i] == '\r') 
        {
            i++; continue;
        }

        // Check to see what type of character this is
        if (strchr(ALPHABET, data[i]))
        {
            // It is an alphabetic character
            std::string str;
            do 
            {   
                // Create a string with all of the characters until a break (whitespace or a non alphanumeric character)
                str.push_back(data[i]);
                i++;
            } while (strchr(/* Identifiers can be alphanumeric after the first character */ALPHANUMERIC, data[i]));

            // If it is a keyword, add it to tokens
            if (map.contains(str)) tokens.push_back(map[str]);
            else 
            {
                // Create a identifier from the string
                Token create = map["[alphan]"];
                create.value = str;
                tokens.push_back(create);
            }
        }  
        else if (strchr(DIGITS, data[i])) 
        {
            std::string numstr;
            do 
            {
                // Create a string with all of the characters until a break (whitespace or a non number character)
                numstr.push_back(data[i]);
                i++;
            } while (strchr(DIGITS, data[i]));

            // Can't have character in a number
            if (strchr(ALPHABET, data[i])) throw compiler_error("%c is not an expected digit", data[i]);
            
            // Create the number token
            Token create = map["[integer]"];
            create.value = numstr;
            tokens.push_back(create);
        }
        else
        {
            // A character group can at most be 3 characters
            if (map.contains(data.substr(i, 3))) 
            {
                tokens.push_back(map[data.substr(i, 3)]);
                tokens.back().value = data.substr(i, 3);   
                i+=3;  
            }     
            else if (map.contains(data.substr(i, 2))) 
            {
                tokens.push_back(map[data.substr(i, 2)]);  
                tokens.back().value = data.substr(i, 2);   
                i+=2;
            }      
            else if (map.contains(data.substr(i, 1))) 
            {
                tokens.push_back(map[data.substr(i, 1)]);
                tokens.back().value = data.substr(i, 1);   
                i++;
            }          
            else throw compiler_error("Invalid expression: %c", data[i]);
        }
    }

    return Tokenizer(std::move(tokens));
}