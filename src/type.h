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
    TypeKind t_kind;
    size_t size_of;

    bool operator==(const Type& type) const { return this->t_kind == type.t_kind && this->size_of == type.size_of; }
    bool operator!=(const Type& type) const { return !(this->t_kind == type.t_kind && this->size_of == type.size_of); }
    operator bool() { return t_kind != TypeKind::NULLTP; }
};

template <class T>
inline size_t hash_combine(size_t seed, const T& v)
{
    std::hash<T> hasher;
    return seed ^ hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

namespace std {
    template<>
    struct hash<Type> {
        inline size_t operator()(const Type& type) const {
            std::hash<TypeKind> hasher;
            return hash_combine(hasher(type.t_kind), type.size_of);
        }
    };
}

// Generates a type from a literal
Type gen_const_type(Tokenizer& tokens);
// Implicitly casts 2 types
Type impl_cast(const Type& lhs, const Type& rhs);
// Explicitly casts 2 types
Type expl_cast(const Type& lhs, const Type& rhs);
// Generates a type from a explicit type token like float
Type gen_expl_type(Tokenizer& tokens);

// Converts a type to a string
std::string to_string(Type type);
