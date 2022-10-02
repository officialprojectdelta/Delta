#pragma once

#include "lexer/token.h"

// All of the data for typing in the compiler

// The different kinds of types (such as ints, floats, and bools)
enum class TypeKind
{
    INT,
    FLOAT,
    NULLTP
}; 

// The actual type entry in the node or symtable entry 
struct Type
{
    TypeKind tKind;
    size_t size_of;

    bool operator==(const Type& type) const { return this->tKind == type.tKind && this->size_of == type.size_of; }
    bool operator!=(const Type& type) const { return !(this->tKind == type.tKind && this->size_of == type.size_of); }
    operator bool() { return tKind != TypeKind::NULLTP; }
};

Type genConstType(Tokenizer& tokens);
Type implCastable(const Type& lhs, const Type& rhs);
Type exprCast(const Type& lhs, const Type& rhs);
Type genExplType(Tokenizer& tokens);
