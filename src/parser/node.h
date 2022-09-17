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
    IF,
    ELSE,
    ELIF, // TODO later, more efficient else if statements (only one jump instead of 2)
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
    NOKIND
};

// The Node struct
// Holds data for a parser node

struct Node
{
    NodeKind kind;
    
    Type type;
    Token tok;

    std::vector<Node> forward; // The nodes that this node points to. Amount depends on NodeKind

    Node* back;

    Node(NodeKind kind, Node* back) { this->kind = kind; this->back = back; }
    Node(NodeKind kind) { this->kind = kind; this->back = nullptr; }
    Node() { this->kind = NodeKind::NOKIND; this->back = nullptr; }
};