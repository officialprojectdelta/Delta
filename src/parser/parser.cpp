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
std::unordered_map</* Binary operator */ TokenType, std::pair<size_t /* Precedence */, bool /* Left or right */ >> prec_map({
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
ProgramNode* parse_program(Tokenizer& tokens);
FunctionNode* parse_function(Tokenizer& tokens);
Node* parse_blk_item(Tokenizer& tokens);
Node* parse_statement(Tokenizer& tokens);
Node* parse_exp(Tokenizer& tokens, size_t min_prec);
Node* parse_atom(Tokenizer& tokens);
Node* parse_base_atom(Tokenizer& tokens);
DeclNode* do_decl(Tokenizer& tokens);
bool check_type(Tokenizer& tokens);

ProgramNode* parse_program(Tokenizer& tokens)
{
    ProgramNode* current = new ProgramNode();

    while (tokens.getPos() < tokens.size())
    {
        size_t before_type = tokens.getPos();
        gen_expl_type(tokens);
        size_t after_type = tokens.getPos();
        tokens.setPos(before_type);

        // FIX FINDING PARENTHESIS FOR FUNCTION
        if (tokens.cur(after_type + 1 - before_type).type == TokenType::OPAREN)
        {
            // A program node will create a function subnode
            current->forward.emplace_back(parse_function(tokens));
            tokens.inc();
        }
        else 
        {
            // Is declaration
            current->forward.emplace_back(do_decl(tokens));
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of declaration %s", tokens.cur().value.c_str());
            tokens.inc();
        }
    }

    return current;
}

FunctionNode* parse_function(Tokenizer& tokens)
{   
    FunctionNode* current = new FunctionNode;

    Type type = gen_expl_type(tokens);
    if (type.t_kind == TypeKind::NULLTP) throw compiler_error("Expected return type of function before identifier");
    if (tokens.cur().type != TokenType::IDENT) throw compiler_error("Expected identifier or \'(\' before \'%s\' token", tokens.cur().value.c_str());

    current->type = type;
    current->name = tokens.cur();

    tokens.inc();

    if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Invalid function declaration");
    tokens.inc();

    if (tokens.cur().type == TokenType::CPAREN) tokens.inc();
    else
    {
        while (true)
        {
            current->args.emplace_back();
            Type type = gen_expl_type(tokens);
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

    // Loop through func (which is a list of statements), if } is found end the loop
    while (true) 
    {
        tokens.check("Invalid function declaration");

        // Parse blkitem should point to next token 
        if (tokens.cur().type == TokenType::CBRACKET) return current;

        current->statements.forward.emplace_back(parse_blk_item(tokens));
    }

    return current;
}

// Check for declaration, used for blkitems, global scope, and loops
DeclNode* do_decl(Tokenizer& tokens)
{
    DeclNode* decl = new DeclNode;
    decl->type = gen_expl_type(tokens);
    decl->name = tokens.cur();
    tokens.inc();

    // Check if it is just a declaration or an assignment
    if (tokens.cur().type == TokenType::ASSIGN)
    {
        tokens.inc();
        decl->assign = parse_exp(tokens, 0);
    }

    return decl;
}

bool check_type(Tokenizer& tokens)
{
    size_t pos = tokens.getPos();
    bool retVal = gen_expl_type(tokens).t_kind != TypeKind::NULLTP;
    tokens.setPos(pos);
    return retVal;
}

// Make sure declarations arn't in if statements that don't create a new scope
// So there is no if variable creation
Node* parse_blk_item(Tokenizer& tokens)
{
    // Check for declaration
    if (check_type(tokens))
    {
        DeclNode* decl = do_decl(tokens);
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of declaration");
        tokens.inc();
        return decl;
    }
    // Parse as a statement
    else return parse_statement(tokens);
}

Node* parse_statement(Tokenizer& tokens)
{
    // Return statement
    if (tokens.cur().type == TokenType::RET)
    {
        RetNode* rnode = new RetNode;
        tokens.inc();

        rnode->value = parse_exp(tokens, 0);

        // Make sure a return statement ends in a semicolon
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();
        return rnode;
    }
    // If statement
    if (tokens.cur().type == TokenType::IF)
    {
        IfNode* inode = new IfNode;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        // Get condition expression
        if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression");
        inode->condition = parse_exp(tokens, 0);

        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        tokens.inc();

        // Get statement to execute if condition is true
        inode->statement = parse_statement(tokens);

        // Check if next token is else
        if (tokens.cur().type == TokenType::ELSE) 
        { 
            tokens.inc();
            inode->else_stmt = parse_statement(tokens);
        }

        return inode;
    }
    else if (tokens.cur().type == TokenType::FOR)
    {
        ForNode* fnode = new ForNode;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        // Parse initial statement
        if (check_type(tokens)) 
        {
            fnode->initial = do_decl(tokens);
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
            tokens.inc();
        }
        else
        {
            fnode->initial = parse_exp(tokens, 0);
            if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
            tokens.inc();
        }

        // Parse break loop if false statement
        fnode->condition = parse_exp(tokens, 0);
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of expression");
        tokens.inc();

        if (tokens.cur().type != TokenType::CPAREN)
        {
            fnode->end = parse_exp(tokens, 0);
            if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
            tokens.inc();
        }
        else
        {
            fnode->end = new NoExpr;
            tokens.inc();
        }

        // Get statement to execute if condition is true
        fnode->statement = parse_statement(tokens);

        return fnode;
    }
    else if (tokens.cur().type == TokenType::WHILE)
    {   
        WhileNode* wnode = new WhileNode;
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression");

        // Parse condition
        wnode->condition = parse_exp(tokens, 0);

        // Check for errors
        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        tokens.inc();

        // Get statement to execute if condition is true
        wnode->statement = parse_statement(tokens);

        return wnode;
    }
    else if (tokens.cur().type == TokenType::DO)
    {
        WhileNode* wnode = new WhileNode;
        wnode->do_on = true;
        tokens.inc();

        // Get statement to execute if condition is true
        wnode->statement = parse_statement(tokens);

        // Look for while, if not found error
        if (tokens.cur().type != TokenType::WHILE) throw compiler_error("Expected 'while' in do/while loop");
        tokens.inc();
        if (tokens.cur().type != TokenType::OPAREN) throw compiler_error("Expected '(' before expr %d", (size_t) tokens.cur().type);
        tokens.inc();

        if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression");

        wnode->condition = parse_exp(tokens, 0);

        // Check for errors
        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched \'(\'");
        tokens.inc();

        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");
        tokens.inc();

        return wnode;
    }
    else if (tokens.cur().type == TokenType::BREAK)
    {
        tokens.inc();
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");
        tokens.inc();

        return new BreakNode;
    }
    else if (tokens.cur().type == TokenType::CONTINUE)
    {
        tokens.inc();
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");
        tokens.inc();

        return new ContinueNode;
    }
    else if (tokens.cur().type == TokenType::OBRACKET)
    {
        BlockStmtNode* bnode = new BlockStmtNode;
        tokens.inc();

        if (tokens.cur().type == TokenType::CBRACKET) return bnode;

        // Loop through func (which is a list of statements), if } is found end the loop
        while (true) 
        {
            tokens.check("Invalid block statement");

            // ParseBlk item should point to next token
            if (tokens.cur().type == TokenType::CBRACKET) break;

            // This is evaluated in the parse_statement function
            bnode->forward.emplace_back(parse_blk_item(tokens));
        }

        tokens.inc();

        return bnode;
    }
    // Is expression (error handling done in parse_exp)
    else
    {
        Node* node = parse_exp(tokens, 0);
        // Make sure a statement ends in a semicolon
        if (tokens.cur().type != TokenType::SEMI) throw compiler_error("Expected end of statement");

        tokens.inc();
        return node;
    }
}

Node* parse_exp(Tokenizer& tokens, size_t min_prec)
{   
    if (tokens.cur().type == TokenType::SEMI) return new NoExpr;

    Node* lhs = parse_atom(tokens);

    while (true)
    {
        if (!prec_map.contains(tokens.cur().type) || prec_map[tokens.cur().type].first < min_prec) break;

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
            TernNode* tern = new TernNode;
            tern->condition = lhs;
            lhs = tern;
            tern->forceboolout = false;

            tokens.inc();   

            tern->lhs = parse_exp(tokens, 0);

            if (tokens.cur().type != TokenType::COLON) throw compiler_error("Expected : to match ?");
            tokens.inc();

            tern->rhs = parse_exp(tokens, 0);
        }
        else if (tokens.cur().type == TokenType::OR || tokens.cur().type == TokenType::AND)
        {
            // Build OR and AND as a ternary node because it is less code
            TernNode* tern = new TernNode;
            tern->condition = lhs;
            lhs = tern;
            tern->forceboolout = true;

            auto precAssoc = prec_map[tokens.cur().type];
            size_t nextMinimumPrec = precAssoc.second ? precAssoc.first : precAssoc.first + 1;

            if (tokens.cur().type == TokenType::OR)
            {
                LiteralNode* lhs = new LiteralNode;
                lhs->type = Type{TypeKind::BOOL, 1};
                lhs->value = Token{TokenType::INTV, "1"};

                tern->lhs = lhs;
                tokens.inc();
                if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression before semicolon");
                tern->rhs = parse_exp(tokens, nextMinimumPrec);
            }
            else
            {
                LiteralNode* rhs = new LiteralNode;
                rhs->type = Type{TypeKind::BOOL, 1};
                rhs->value = Token{TokenType::INTV, "0"};

                tern->rhs = rhs;
                tokens.inc();
                if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression before semicolon");
                tern->lhs = parse_exp(tokens, nextMinimumPrec);
            }
        }
        else
        {
            BinaryOpNode* rval = new BinaryOpNode;
            rval->lhs = lhs;
            lhs = rval;

            rval->op = convert[tokens.cur().type];
            auto precAssoc = prec_map[tokens.cur().type];

            size_t nextMinimumPrec = precAssoc.second ? precAssoc.first : precAssoc.first + 1;

            tokens.inc();
            if (tokens.cur().type == TokenType::SEMI) throw compiler_error("Expected expression before semicolon");

            rval->rhs = parse_exp(tokens, nextMinimumPrec);
        }
    }

    return lhs;
}

Node* parse_atom(Tokenizer& tokens)
{
    if (tokens.cur().type == TokenType::IDENT)
    {
        switch (tokens.next().type)
        {
            case TokenType::INC:
            {
                UnaryOpNode* op = new UnaryOpNode;
                op->op = NodeKind::POSTFIXINC;
                op->forward = parse_base_atom(tokens);
                tokens.inc();
                return op;
            }
            case TokenType::DEC:
            {
                UnaryOpNode* op = new UnaryOpNode;
                op->op = NodeKind::POSTFIXDEC;
                op->forward = parse_base_atom(tokens);
                tokens.inc();
                return op;
            }
            case TokenType::OPAREN:
            {
                // Parse function
                // Subnodes are args
                FuncallNode* fn = new FuncallNode;

                fn->name = tokens.cur();
                
                tokens.inc();
                tokens.inc();

                while (tokens.cur().type != TokenType::CPAREN)
                {
                    fn->args.emplace_back(parse_exp(tokens, 0));

                    if (tokens.cur().type == TokenType::CPAREN) break;
                    if (tokens.cur().type != TokenType::COMMA) throw compiler_error("Expected comma before next argument");
                    tokens.inc();
                }

                tokens.inc();
                return fn;
            }
        }
    }

    return parse_base_atom(tokens);
}

Node* parse_base_atom(Tokenizer& tokens)
{
    if (tokens.cur().type == TokenType::OPAREN) 
    {
        tokens.inc();

        Node* exp = parse_exp(tokens, 0);

        if (tokens.cur().type != TokenType::CPAREN) throw compiler_error("Unmatched parenthesis \'(\'");
        else 
        {
            tokens.inc();
            return exp;
        }
    }
    else
    {
        size_t pos = tokens.getPos();
        Type type = gen_const_type(tokens);
        if (type.t_kind != TypeKind::NULLTP) 
        {
            LiteralNode* lit = new LiteralNode;
            lit->type = type;
            lit->value = tokens.cur();    
            tokens.inc();
            return lit;
        }
        else 
        {
            tokens.setPos(pos);

            if (tokens.cur().type == TokenType::IDENT)
            {
                VarNode* var = new VarNode;
                var->name = tokens.cur();
                tokens.inc();
                return var;
            }
            else 
            {
                UnaryOpNode* op = new UnaryOpNode;

                std::unordered_map<TokenType, NodeKind> convert({
                    {TokenType::NOT, NodeKind::NOT},
                    {TokenType::DASH, NodeKind::NEG},
                    {TokenType::BITCOMPL, NodeKind::BITCOMPL},
                    {TokenType::INC, NodeKind::PREFIXINC},
                    {TokenType::DEC, NodeKind::PREFIXDEC},
                });

                if (!convert.contains(tokens.cur().type)) throw compiler_error("Couldn't build an atom from: %s", tokens.cur().value.c_str()); 
                op->op = convert[tokens.cur().type];
                tokens.inc();
    
                op->forward = parse_atom(tokens);

                return op;            
            }
        }
    }

    throw compiler_error("Couldn't build an atom from: %s", tokens.cur().value.c_str());

    return nullptr;
}
    