#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <list>

#include "lexer/token.h"
#include "type.h"

enum class NodeKind
{   
    PRGRM, 
    FUNCTION, 
    FUNCALL,
    ARG,
    RETURN,
    BLOCKSTMT,
    IF,
    ELSE,
    FOR,
    WHILE,
    DO,
    BREAK,
    CONTINUE,
    ADD, 
    SUB,
    MUL, 
    DIV, 
    MOD,
    NOT, 
    NEG, 
    BITCOMPL,
    PREFIXINC,
    PREFIXDEC,
    POSTFIXINC,
    POSTFIXDEC,
    AND,
    OR,
    EQ,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
    TERN,
    ASSIGN, 
    LIT, 
    DECL,
    VAR, 
    NOEXPR,
    NOKIND
};

enum class Location
{
    LOCAL, GLOBAL, FUNCTION
};

// The Node struct
// Holds data for a parser node

struct Node
{
    // For every node, will codegen output of the nodetype
    virtual void visit(std::string* write) = 0;
    virtual ~Node() = 0;
};

struct ProgramNode : Node
{
    Node* back;

    // List of every node, going forward
    std::list<Node> forward;

    // Codegen
    void visit(std::string* write) override;

    ProgramNode(Node* back) { this->back = back; }
    ProgramNode() { this->back = nullptr; }
};

struct ArgNode : Node
{
    Node* back;

    // Type of argument
    Type type;
    
    // Name of argument value
    Token tok;

    // Codegen
    void visit(std::string* write) override;

    ArgNode(Node* back) { this->back = back; }
    ArgNode() { this->back = nullptr; }
};

struct BlockStmtNode : Node
{   
    Node* back;

    // Every statment in the block statment
    std::list<Node> forward;

    // Codegen
    void visit(std::string* write) override;

    BlockStmtNode(Node* back) { this->back = back; }
    BlockStmtNode() { this->back = nullptr; }
};

struct FunctionNode : Node
{
    Node* back;

    // Return type of the function
    Type type; 

    // Function name
    Token tok;

    // List of arguments
    std::list<ArgNode> args;

    // List of every node (instruction) going forward
    std::list<BlockStmtNode> forward;

    // Codegen
    void visit(std::string* write) override;

    FunctionNode(Node* back) { this->back = back; }
    FunctionNode() { this->back = nullptr; }
};

struct ExpNode : Node 
{
    Node* back;
    virtual void visit(std::string* write) override = 0;
    virtual ~ExpNode() = override 0;
}

struct BinaryOpNode : ExpNode
{
    ExpNode* lhs;
    ExpNode* rhs;

    ~BinaryOpNode() override 
    {
        if (lhs) delete lhs;
        if (rhs) delete rhs;
    }
}

struct RetNode : Node
{
    Node* back;

    // The return value of the statment
    ExpNode* value;

    // Codegen
    void visit(std::string* write) const override;

    Node(Node* back) { this->back = back; }
    Node() { this->back = nullptr; }
    ~RetNode() { if (value) delete value; } override;
};