#include "parser.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <array>

#include "util.h"
#include "error/error.h"

// Precedence map for binary input operations
std::unordered_map</* Binary operator */ TokenType, std::pair<size_t /* Precedence */, bool /* Left or right */ >> precMap({
    {TokenType::ADD, {5, 0}}, 
    {TokenType::DASH, {5, 0}},
    {TokenType::MUL, {6, 0}},
    {TokenType::DIV, {6, 0}},
    {TokenType::AND, {2, 0}},
    {TokenType::OR, {1, 0}},
    {TokenType::EQ, {3, 0}},
    {TokenType::NOTEQ, {3, 0}},
    {TokenType::LESS, {4, 0}},
    {TokenType::LESSEQ, {4, 0}},
    {TokenType::GREATER, {4, 0}},
    {TokenType::GREATEREQ, {4, 0}},
    {TokenType::ASSIGN, {0, 1}},
});

// List of types (only contains int for now)
std::array<TokenType, 1> types({
    TokenType::TINT
});

// Function definitions not all are needed, but they are all here (some are needed)
Node* parseProgram(Node* current, Tokenizer& tokens);
Node* parseFunction(Node* current, Tokenizer& tokens);
Node* parseStatement(Node* current, Tokenizer& tokens);
Node* parseExp(Node* current, Tokenizer& tokens, size_t min_prec);
Node* parseAtom(Node* current, Tokenizer& tokens);

Node* parseProgram(Node* current, Tokenizer& tokens)
{
    current->kind = NodeKind::PRGRM;

    // A program node will create a function subnode
    current->forward.emplace_back(NodeKind::FUNCTION, current);
    
    parseFunction(&current->forward.back(), tokens);

    return current;
}

Node* parseFunction(Node* current, Tokenizer& tokens)
{   
    if (tokens.cur().type == TokenType::TINT)
    {
        current->type.typeKind = TypeKind::INT;
        current->type.sizeofNode = 4;
        current->type.issigned = true;
        current->type.ptrCount = 0;
    }
    else
    {        
        throw compiler_error("Unknown type name: %s", tokens.cur().value.c_str());
    }

    tokens.inc();

    if (tokens.cur().type != TokenType::IDENT) throw compiler_error("Expected identifier or \'(\' before \'%s\' token", tokens.cur().value.c_str());

    current->tok = tokens.cur();

    tokens.inc();

    if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Invalid function declaration");
    tokens.inc();
    if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Invalid function declaration");
    tokens.inc();
    if (tokens.cur().type != TokenType::OBRACKET) throw compiler_error("Invalid function declaration");
    tokens.inc();

    // Loop through func (which is a list of statements), if } is found end the loop
    while (true) 
    {
        if (tokens.cur().type == TokenType::CBRACKET) return current;

        // This is evaluated in the parseStatement function
        current->forward.emplace_back(NodeKind::NOKIND, current);
    
        parseStatement(&current->forward.back(), tokens);

        // Increment tokens before restarting the loop
        tokens.inc(); 

        tokens.check("Invalid function declaration");
    }
}

Node* parseStatement(Node* current, Tokenizer& tokens)
{
    if (tokens.cur().type == TokenType::RET)
    {
        current->kind = NodeKind::RETURN;
        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);

        parseExp(&current->forward.back(), tokens, 0);
    }
    // Check if it is a declaration
    else if (std::find(types.begin(), types.end(), tokens.cur().type) != types.end())
    {
        current->kind = NodeKind::DECL;
        current->type = Type({TypeKind::INT, 4, true, 0});
        tokens.inc();
        current->tok = tokens.inc();

        // Check if it is just a declaration or an assignment
        if (tokens.cur().type == TokenType::ASSIGN)
        {
            tokens.inc();
            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, 0);
        }
    }
    // Is expression (error handling done in parseexp)
    else
    {
        parseExp(current, tokens, 0);
    }

    if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

    return current;
}

Node* parseExp(Node* current, Tokenizer& tokens, size_t min_prec)
{   
    parseAtom(current, tokens);

    while (true)
    {
        if (!precMap.contains(tokens.cur().type) || precMap[tokens.cur().type].first < min_prec) break;

        std::unordered_map<TokenType, NodeKind> convert({
            {TokenType::ADD, NodeKind::ADD},
            {TokenType::DASH, NodeKind::SUB},
            {TokenType::MUL, NodeKind::MUL},
            {TokenType::DIV, NodeKind::DIV}, 
            {TokenType::AND, NodeKind::AND},
            {TokenType::OR, NodeKind::OR},
            {TokenType::EQ, NodeKind::EQ},
            {TokenType::NOTEQ, NodeKind::NOTEQ},
            {TokenType::LESS, NodeKind::LESS},
            {TokenType::LESSEQ, NodeKind::LESSEQ},
            {TokenType::GREATER, NodeKind::GREATER},
            {TokenType::GREATEREQ, NodeKind::GREATEREQ}, 
            {TokenType::ASSIGN, NodeKind::ASSIGN}, 
        });

        Node temp = std::move(*current);
        temp.back = current;

        current->forward.emplace_back(std::move(temp));

        current->kind = convert[tokens.cur().type];

        auto precAssoc = precMap[tokens.cur().type];

        size_t nextMinimumPrec = precAssoc.second ? precAssoc.first : precAssoc.first + 1;

        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);

        parseExp(&current->forward.back(), tokens, nextMinimumPrec);
    }

    return current;
}

Node* parseAtom(Node* current, Tokenizer& tokens)
{
    if (tokens.cur().type == TokenType::OPAREN) 
    {
        tokens.inc();

        parseExp(current, tokens, 0);

        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched parenthesis \'(\'");
        else 
        {
            tokens.inc();
            return current;
        }
    }
    else if (tokens.cur().type == TokenType::INTV)
    {
        current->kind = NodeKind::NUM;
        
        current->type.typeKind = TypeKind::INT;
        current->type.sizeofNode = 4;
        current->type.issigned = true;
        current->type.ptrCount = 0;

        current->tok = tokens.cur();

        tokens.inc();

        return current;
    }
    else if (tokens.cur().type == TokenType::IDENT)
    {
        current->kind = NodeKind::VAR;
        current->tok = tokens.cur();

        tokens.inc();

        return current;
    }
    else 
    {
        std::unordered_map<TokenType, NodeKind> convert({
            {TokenType::NOT, NodeKind::NOT},
            {TokenType::DASH, NodeKind::NEG},
            {TokenType::BITCOMPL, NodeKind::BITCOMPL}
        });

        if (!convert.contains(tokens.cur().type)) compiler_error("Couldn't build an atom from: %s", tokens.cur().value.c_str()); 

        current->kind = convert[tokens.cur().type];

        tokens.inc();
                
        current->forward.emplace_back(NodeKind::NOKIND, current);

        parseAtom(&current->forward.back(), tokens);

        return current;            
    }

    throw compiler_error("Couldn't build an atom from: %s", tokens.cur().value.c_str());

    return nullptr;
}
    
// A wrapper for the parseNode function
// Takes in a map and tokens
Node* parse(Tokenizer& tokens)
{
    // Create a node
    Node* node = new Node;
    
    // Parse it as a programNode
    parseProgram(node, tokens);

    // Return the created node pointer
    return node;
}
