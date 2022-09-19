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
    NOT, 
    NEG, 
    BITCOMPL,
    AND,
    OR,
    EQ,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
    ASSIGN, 
    NUM, 
    DECL,
    VAR, 
    NOEXPR,
    NOKIND
};

// The Node struct
// Holds data for a parser node

struct Node
{
    NodeKind kind;
    
    Type type;
    Token tok;
    std::string varName; // Used as the variable name in finding it's location in the varmap

    std::vector<Node> forward; // The nodes that this node points to. Amount depends on NodeKind

    Node* back;

    Node(NodeKind kind, Node* back) { this->kind = kind; this->back = back; }
    Node(NodeKind kind) { this->kind = kind; this->back = nullptr; }
    Node() { this->kind = NodeKind::NOKIND; this->back = nullptr; }
};