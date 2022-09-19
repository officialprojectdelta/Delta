#include "parser.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <array>

#include "util.h"
#include "error/error.h"

// FULLY SUPPORT DEC AND INC  

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
Node* parseBlkitem(Node* current, Tokenizer& tokens);
Node* parseStatement(Node* current, Tokenizer& tokens);
Node* parseExp(Node* current, Tokenizer& tokens, size_t min_prec);
Node* parseAtom(Node* current, Tokenizer& tokens);
Node* parseBaseAtom(Node* current, Tokenizer& tokens);

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
        tokens.check("Invalid function declaration");

        // Parse blkitem should point to next token 
        if (tokens.cur().type == TokenType::CBRACKET) return current;

        // This is evaluated in the parseStatement function
        current->forward.emplace_back(NodeKind::NOKIND, current);
    
        parseBlkitem(&current->forward.back(), tokens);
    }
}

// Check for declaratio, used in loops and 
Node* doDecl(Node* current, Tokenizer& tokens)
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

    // Make sure a statement ends in a semicolon
    if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of declaration");

    tokens.inc();

    return current;
}

bool checkDecl(Tokenizer& tokens)
{
    return std::find(types.begin(), types.end(), tokens.cur().type) != types.end();
}

// Make sure declarations arn't in if statements that don't create a new scope
// So there is no if variable creation
Node* parseBlkitem(Node* current, Tokenizer& tokens)
{
    // Check for declaration
    if (checkDecl(tokens))
    {
        doDecl(current, tokens);
        return current;
    }
    // Parse as a statement
    else
    {
        parseStatement(current, tokens);
        return current;
    }
}

Node* parseStatement(Node* current, Tokenizer& tokens)
{
    // Return statement
    if (tokens.cur().type == TokenType::RET)
    {
        current->kind = NodeKind::RETURN;

        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);

        // Make sure a return statement ends in a semicolon
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();

        return current;
    }
    // If statement
    if (tokens.cur().type == TokenType::IF)
    {
        current->kind = NodeKind::IF;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        // Get condition expression
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);
        if (current->forward.back().kind == NodeKind::NOEXPR)
        {
            throw compiler_error("Expected expression");
        }

        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        tokens.inc();

        // Get statement to execute if condition is true
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseStatement(&current->forward.back(), tokens);

        // Check if next token is else
        if (tokens.next().type == TokenType::ELSE) 
        { 
            tokens.inc();
            tokens.inc();
            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseStatement(&current->forward.back(), tokens);
        }

        return current;
    }
    else if (tokens.cur().type == TokenType::FOR)
    {
        current->kind = NodeKind::FOR;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        if (checkDecl(tokens)) 
        {
            current->forward.emplace_back(NodeKind::NOKIND, current);
            doDecl(&current->forward.back(), tokens);
            tokens.inc();
        }
        else
        {
            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, 0);
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
            tokens.inc();
        }

        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);
        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        tokens.inc();

        // Get statement to execute if condition is true
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseStatement(&current->forward.back(), tokens);

        return current;
        
    }
    else if (tokens.cur().type == TokenType::WHILE)
    {   
        current->kind = NodeKind::WHILE;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);

        // Check for errors
        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        if (current->forward.back().kind == NodeKind::NOEXPR) throw compiler_error("Expected expression");

        tokens.inc();

        // Get statement to execute if condition is true
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseStatement(&current->forward.back(), tokens);

        return current;
    }
    else if (tokens.cur().type == TokenType::DO)
    {
        current->kind = NodeKind::DO;
        tokens.inc();

        // Get statement to execute if condition is true
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseStatement(&current->forward.back(), tokens);

        // Look for while, if not found error
        if (tokens.cur().type != TokenType::WHILE) throw compiler_error("Expected 'while' in do/while loop");
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseExp(&current->forward.back(), tokens, 0);

        // Check for errors
        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        if (current->forward.back().kind == NodeKind::NOEXPR) throw compiler_error("Expected expression");

        tokens.inc();

        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();

        return current;
    }
    else if (tokens.cur().type == TokenType::BREAK)
    {
        current->kind = NodeKind::BREAK;
        tokens.inc();

        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();

        return current;
    }
    else if (tokens.cur().type == TokenType::CONTINUE)
    {
        current->kind = NodeKind::CONTINUE;
        tokens.inc();

        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();

        return current;
    }
    else if (tokens.cur().type == TokenType::OBRACKET)
    {
        current->kind = NodeKind::BLOCKSTMT;
        tokens.inc();

        if (tokens.cur().type == TokenType::CBRACKET) return current;

        // Loop through func (which is a list of statements), if } is found end the loop
        while (true) 
        {
            tokens.check("Invalid block statement");

            // ParseBlk item should point to next token
            if (tokens.cur().type == TokenType::CBRACKET) break;

            // This is evaluated in the parseStatement function
            current->forward.emplace_back(NodeKind::NOKIND, current);
        
            parseBlkitem(&current->forward.back(), tokens);
        }

        tokens.inc();

        return current;
    }
    // Is expression (error handling done in parseexp)
    else
    {
        parseExp(current, tokens, 0);
        // Make sure a statement ends in a semicolon
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();
    }

    return current;
}

Node* parseExp(Node* current, Tokenizer& tokens, size_t min_prec)
{   
    if (tokens.cur().type == TokenType::SEMI)
    {
        current->kind = NodeKind::NOEXPR;
        return current;
    }

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
    if (tokens.cur().type == TokenType::IDENT)
    {
        switch (tokens.next().type)
        {
            case TokenType::INC:
            {
                current->kind = NodeKind::POSTFIXINC;
                current->forward.emplace_back(NodeKind::NOKIND, current);
                parseBaseAtom(&current->forward.back(), tokens);
                tokens.inc();
                return current;
            }
            case TokenType::DEC:
            {
                current->kind = NodeKind::POSTFIXDEC;
                current->forward.emplace_back(NodeKind::NOKIND, current);
                parseBaseAtom(&current->forward.back(), tokens);
                tokens.inc();
                return current;
            }
        }
    }

    parseBaseAtom(current, tokens);
    return current;
}

Node* parseBaseAtom(Node* current, Tokenizer& tokens)
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
            {TokenType::BITCOMPL, NodeKind::BITCOMPL},
            {TokenType::INC, NodeKind::PREFIXINC},
            {TokenType::DEC, NodeKind::PREFIXDEC},
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
