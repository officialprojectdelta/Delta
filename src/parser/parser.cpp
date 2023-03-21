#include "parser.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <array>

#include "util.h"
#include "error/error.h"
#include "type.h"

// Precedence map for binary input operations
std::unordered_map</* Binary operator */ TokenType, std::pair<size_t /* Precedence */, bool /* Left or right */ >> precMap({
    {TokenType::ADD, {6, 0}}, 
    {TokenType::DASH, {6, 0}},
    {TokenType::MUL, {7, 0}},
    {TokenType::DIV, {7, 0}},
    {TokenType::MOD, {7, 0}}, 
    {TokenType::AND, {3, 0}},
    {TokenType::OR, {2, 0}},
    {TokenType::EQ, {4, 0}},
    {TokenType::NOTEQ, {4, 0}},
    {TokenType::LESS, {5, 0}},
    {TokenType::LESSEQ, {5, 0}},
    {TokenType::GREATER, {5, 0}},
    {TokenType::GREATEREQ, {5, 0}},
    {TokenType::TERN, {1, 1}},
    {TokenType::ASSIGN, {0, 1}},
});

// Function definitions not all are needed, but they are all here (some are needed)
ProgramNode* parseProgram(Tokenizer& tokens);
Node* parseFunction(FunctionNode* current, Tokenizer& tokens);
Node* parseBlkitem(Node* current, Tokenizer& tokens);
Node* parseStatement(Node* current, Tokenizer& tokens);
ExpNode* parseExp(Tokenizer& tokens, size_t min_prec);
Node* parseAtom(Node* current, Tokenizer& tokens);
Node* parseBaseAtom(Node* current, Tokenizer& tokens);
DeclNode* doDecl(DeclNode* current, Tokenizer& tokens);
bool checkType(Tokenizer& tokens);

ProgramNode* parseProgram(Tokenizer& tokens)
{
    ProgramNode* current = new ProgramNode();

    while (tokens.getPos() < tokens.size())
    {
        if (tokens.cur(2).type == TokenType::OPAREN)
        {
            // A program node will create a function subnode
            FunctionNode* fn = new FunctionNode(current);
            current->forward.emplace_back(fn);
            parseFunction(fn, tokens);
            tokens.inc();
        }
        else 
        {
            // Is declaration
            DeclNode* decl = new DeclNode(current);
            current->forward.emplace_back(decl);
            doDecl(decl, tokens);
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of declaration %s", tokens.cur().value.c_str());
            tokens.inc();
        }
    }

    return current;
}

Node* parseFunction(FunctionNode* current, Tokenizer& tokens)
{   
    Type type = genExplType(tokens);
    if (type.tKind == TypeKind::NULLTP) throw compiler_error("Expected return type of function before identifier");
    if (tokens.cur().type != TokenType::IDENT) throw compiler_error("Expected identifier or \'(\' before \'%s\' token", tokens.cur().value.c_str());

    current->type = type;
    current->tok = tokens.cur();

    tokens.inc();

    if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Invalid function declaration");
    tokens.inc();

    if (tokens.cur().type == TokenType::CPAREN) tokens.inc();
    else
    {
        while (true)
        {
            current->args.emplace_back(current);
            Type type = genExplType(tokens);
            if (!type) throw compiler_error("Expected type of arg before identifier");
            if (tokens.cur().type != TokenType::IDENT) throw compiler_error("Expected identifier as argument");
            
            current->args.back().type = type;
            current->args.back().tok = tokens.cur();

            tokens.inc();

            if (tokens.cur().type == TokenType::CPAREN)
            {
                tokens.inc();
                break;
            } 

            if (tokens.cur().type != TokenType::COMMA) throw compiler_error("Expected seperator before next argument");
            tokens.inc();
        }
    }

    if (tokens.cur().type == TokenType::SEMI) return current;

    if (tokens.cur().type != TokenType::OBRACKET) throw compiler_error("Invalid function declaration");
    tokens.inc();

    current->forward.emplace_back(current);

    // Loop through func (which is a list of statements), if } is found end the loop
    while (true) 
    {
        tokens.check("Invalid function declaration");

        // Parse blkitem should point to next token 
        if (tokens.cur().type == TokenType::CBRACKET) return current;

        // Pass in block that it is a part of 
        parseBlkitem(&current->forward.back(), tokens);
        
    }

    return current;
}

// Check for declaration, used for blkitems, global scope, and loops
DeclNode* doDecl(DeclNode* current, Tokenizer& tokens)
{
    current->type = genExplType(tokens);
    current->tok = tokens.cur();
    tokens.inc();

    // Check if it is just a declaration or an assignment
    if (tokens.cur().type == TokenType::ASSIGN)
    {
        tokens.inc();
        current->assign = parseExp(tokens, 0);
    }

    return current;
}

bool checkType(Tokenizer& tokens)
{
    size_t pos = tokens.getPos();
    bool retVal = genExplType(tokens).tKind != TypeKind::NULLTP;
    tokens.setPos(pos);
    return retVal;
}

// Make sure declarations arn't in if statements that don't create a new scope
// So there is no if variable creation
Node* parseBlkitem(Node* current, Tokenizer& tokens)
{
    // Check for declaration
    if (checkType(tokens))
    {
        doDecl(current, tokens);
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of declaration");
        tokens.inc();
        return current;
    }
    // Parse as a statement
    else
    {
        parseStatement(current, tokens);
        return current;
    }
}

Node* parseStatement(BlockStmtNode* current, Tokenizer& tokens)
{
    // Return statement
    if (tokens.cur().type == TokenType::RET)
    {
        RetNode* node = new RetNode(current);
        tokens.inc();

        node->value = parseExp(tokens, 0);

        // Make sure a return statement ends in a semicolon
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        current->forward.emplace_back(node);

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

        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched ass \'(\'");
        tokens.inc();

        // Get statement to execute if condition is true
        current->forward.emplace_back(NodeKind::NOKIND, current);
        parseStatement(&current->forward.back(), tokens);

        // Check if next token is else
        if (tokens.cur().type == TokenType::ELSE) 
        { 
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

        if (checkType(tokens)) 
        {
            current->forward.emplace_back(NodeKind::NOKIND, current);
            doDecl(&current->forward.back(), tokens);
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
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

        if (tokens.cur().type != TokenType::CPAREN)
        {
            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, 0);
            if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
            tokens.inc();
        }
        else
        {
            current->forward.emplace_back(NodeKind::NOEXPR, current);
            tokens.inc();
        }

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

ExpNode* parseExp(Tokenizer& tokens, size_t min_prec)
{   
    ExpNode* rval;

    if (tokens.cur().type == TokenType::SEMI)
    {
        rval = new NoExpr(rval);
        return current;
    }

    parseAtom(current, tokens);

    while (true)
    {
        if (!precMap.contains(tokens.cur().type) || precMap[tokens.cur().type].first < min_prec) 
        {
            break;
        }

        std::unordered_map<TokenType, NodeKind> convert({
            {TokenType::ADD, NodeKind::ADD},
            {TokenType::DASH, NodeKind::SUB},
            {TokenType::MUL, NodeKind::MUL},
            {TokenType::DIV, NodeKind::DIV}, 
            {TokenType::MOD, NodeKind::MOD}, 
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

        if (tokens.cur().type == TokenType::TERN)
        {
            Node temp = std::move(*current);
            temp.back = current;

            current->forward.emplace_back(std::move(temp));
            current->kind = NodeKind::TERN;

            tokens.inc();

            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, 0);

            std::cout << "parsing tern" << std::endl;

            if (tokens.cur().type != TokenType::COLON) throw compiler_error("Expected : to match ?");
            tokens.inc();

            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, 0);
        }
        else
        {
            Node temp = std::move(*current);
            temp.back = current;

            current->forward.emplace_back(std::move(temp));

            current->kind = convert[tokens.cur().type];

            auto precAssoc = precMap[tokens.cur().type];

            size_t nextMinimumPrec = precAssoc.second ? precAssoc.first : precAssoc.first + 1;

            tokens.inc();

            current->forward.emplace_back(NodeKind::NOKIND, current);
            parseExp(&current->forward.back(), tokens, nextMinimumPrec);

            if (current->forward.back().kind == NodeKind::NOEXPR) throw compiler_error("Expected expression before semicolon");
        }
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
            case TokenType::OPAREN:
            {
                // Parse function
                // Subnodes are args

                current->kind = NodeKind::FUNCALL;
                current->tok = tokens.cur();
                
                tokens.inc();
                tokens.inc();

                while (tokens.cur().type != TokenType::CPAREN)
                {
                    current->forward.emplace_back(NodeKind::NOKIND, current);
                    parseExp(&current->forward.back(), tokens, 0);

                    if (tokens.cur().type == TokenType::CPAREN) break;

                    if (tokens.cur().type != TokenType::COMMA) throw compiler_error("Expected comma before next argument");
                    tokens.inc();
                }

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
    else
    {
        size_t pos = tokens.getPos();
        Type type = genConstType(tokens);
        if (type.tKind != TypeKind::NULLTP) 
        {
            current->type = type;
            current->kind = NodeKind::LIT;
            current->tok = tokens.cur();    
            tokens.inc();
            return current;
        }
        else 
        {
            tokens.setPos(pos);

            if (tokens.cur().type == TokenType::IDENT)
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

                if (!convert.contains(tokens.cur().type)) throw compiler_error("Couldn't build an atom from: %s", tokens.cur().value.c_str()); 

                current->kind = convert[tokens.cur().type];

                tokens.inc();
                        
                current->forward.emplace_back(NodeKind::NOKIND, current);

                parseAtom(&current->forward.back(), tokens);

                return current;            
            }
        }
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
