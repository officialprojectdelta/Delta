#pragma once

#include <string>
#include <vector>
#include <iostream>

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
    NodeKind kind;
    
    Type type;
    Token tok;
    
    std::string varName; // Used as the variable name in finding it's location in the varmap
    std::string fnName; // Used if variable is in a function, or name of function for function call
    Location loc; // If var is local, function argument, or global

    std::vector<Node> forward; // The nodes that this node points to. Amount depends on NodeKind

    Node* back;

    Node(NodeKind kind, Node* back) { this->kind = kind; this->back = back; }
    Node(NodeKind kind) { this->kind = kind; this->back = nullptr; }
    Node() { this->kind = NodeKind::NOKIND; this->back = nullptr; }
};