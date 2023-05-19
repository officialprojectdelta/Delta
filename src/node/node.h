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
    AND,
    OR,
    NOT, 
    NEG, 
    BITCOMPL,
    PREFIXINC,
    PREFIXDEC,
    POSTFIXINC,
    POSTFIXDEC,
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
    ADDR,
    DEREF, 
    NOEXPR,
    NOKIND
};

enum class Location
{
    LOCAL, GLOBAL, FUNCTION
};

// Holds additional data that is passed into the visit functions so they have some context
struct AdditionalArgs
{
    // Used if the visit function its coming from is a decl function
    bool decl;
};

// The Node struct
// Holds data for a parser node

struct Node
{
    // For every node, will codegen output of the nodetype
    virtual void visit(std::string* write) = 0;
    virtual void visit_symt() {}
    virtual ~Node() = 0;
};

struct ProgramNode : Node
{
    // List of every node, going forward
    std::list<std::unique_ptr<Node>> forward;

    // Codegen
    virtual void visit(std::string* write) override;
    void visit_symt() override
    {
        for (auto i = std::begin(forward); i != std::end(forward); i++)
        {
            (*i)->visit_symt();
        }
    }

    ~ProgramNode() override {}
};

struct ArgNode : Node
{
    // Type of argument
    Type type;
    
    // Name of argument value
    Token tok;

    // Codegen
    virtual void visit(std::string* write) override;

    ~ArgNode() override {}
};

struct BlockStmtNode : Node
{   
    // Every statment in the block statment
    std::list<std::unique_ptr<Node>> forward;

    // Codegen
    virtual void visit(std::string* write) override;

    ~BlockStmtNode() override {}
};

// This is used for generating proper terminator
struct TerminatorCheckNode : Node
{   
    // Every statment in the block statment
    Node* forward;

    // Codegen
    virtual void visit(std::string* write) override;

    ~TerminatorCheckNode() override { if (forward) delete forward; }
};

struct FunctionNode : Node
{
    // Return type of the function
    Type type; 

    // Function name
    Token name;

    // If the function is defined or not
    bool defined = false;

    // List of arguments
    std::vector<ArgNode> args;

    // List of every node (instruction) going forward
    BlockStmtNode statements;

    // Codegen
    virtual void visit(std::string* write) override;
    void visit_symt() override;

    ~FunctionNode() override {}
};

struct NoExpr : Node
{
    virtual void visit(std::string* write) override;

    ~NoExpr() override {}
};

struct CastNode : Node
{
    Node* forward = nullptr;

    Type type;

    virtual void visit(std::string* write) override;

    ~CastNode() override { if (forward) delete forward; }   
};

struct UnaryOpNode : Node
{
    Node* forward = nullptr;

    NodeKind op;

    virtual void visit(std::string* write) override;

    ~UnaryOpNode() override
    {
        if (forward) delete forward;
    }
};

struct BinaryOpNode : Node
{
    Node* lhs = nullptr;
    Node* rhs = nullptr;

    NodeKind op;

    virtual void visit(std::string* write) override;
    
    ~BinaryOpNode() override 
    {
        if (lhs) delete lhs;
        if (rhs) delete rhs;
    }
};

struct TernNode : Node
{
    Node* condition = nullptr;
    Node* lhs = nullptr;
    Node* rhs = nullptr;

    // This is used because we can make AND and OR into ternary, and this makes the output a boolean value like && and ||
    bool forceboolout = false;

    virtual void visit(std::string* write) override;

    ~TernNode() override
    {
        if (condition) delete condition;
        if (lhs) delete lhs;
        if (rhs) delete rhs;
    }
};

struct LiteralNode : Node
{
    Type type;
    Token value;

    virtual void visit(std::string* write) override;
    
    ~LiteralNode() override {}
};

struct VarNode : Node
{
    Token name; 

    virtual void visit(std::string* write) override;

    ~VarNode() override {}
};

struct FuncallNode : Node
{
    Token name;

    std::list<std::unique_ptr<Node>> args;

    virtual void visit(std::string* write) override;

    ~FuncallNode() override {}
};

struct DeclNode : Node
{
    // Type of the variable that is being declared
    Type type; 

    // Variable name
    Token name;

    // If the variable is defined
    bool defined = false;

    // The size of the array, if it is one
    size_t array = 0;

    // Assignment expression if there is one
    Node* assign = nullptr;

    // Array assignment expressions if there is an array assignment
    std::vector<Node*> array_assign;

    virtual void visit(std::string* write) override;
    void visit_symt() override;

    ~DeclNode() override { if (assign) delete assign; }
};

// Node types for break and continue
struct BreakNode : Node { virtual void visit(std::string* write) override; ~BreakNode() override {} };
struct ContinueNode : Node { virtual void visit(std::string* write) override; ~ContinueNode() override {} };

struct RetNode : Node
{
    // The return value of the statement
    Node* value = nullptr;

    // Codegen
    virtual void visit(std::string* write) override;

    ~RetNode() override { if (value) delete value; };
};

struct IfNode : Node
{
    Node* condition = nullptr;
    Node* statement = nullptr;
    Node* else_stmt = nullptr;

    virtual void visit(std::string* write) override;

    ~IfNode() override
    { 
        if (condition) delete condition;
        if (statement) delete statement;
        if (else_stmt) delete else_stmt;
    }
};

struct ForNode : Node
{
    Node* initial = nullptr;
    Node* condition = nullptr;
    Node* end = nullptr;
    Node* statement = nullptr;

    virtual void visit(std::string* write) override;

    ~ForNode() override
    { 
        if (initial) delete initial;
        if (condition) delete condition;
        if (end) delete end;
        if (statement) delete statement;
    }
};

struct WhileNode : Node
{
    Node* condition = nullptr;
    Node* statement = nullptr;

    // So I can reuse this for do, because do and while are very similar
    bool do_on = false;

    virtual void visit(std::string* write) override;

    ~WhileNode() override
    { 
        if (condition) delete condition;
        if (statement) delete statement;
    }
};