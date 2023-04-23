#pragma once

#include "lexer/token.h"

// std
#include <unordered_map>
#include <string>

// All of the functions for typing in the compiler

// The different kinds of types (such as ints, floats, and bools)
enum class TypeKind
{
    INT,
    FLOAT,
    BOOL, 
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

struct TwoType
{
    Type lhs;
    Type rhs;

    bool operator==(const TwoType& type) const { return this->lhs == type.lhs && this->rhs == type.rhs; }
};

template <class T>
inline size_t hash_combine(size_t seed, const T& v)
{
    std::hash<T> hasher;
    return seed ^ hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// Hashing stuff
namespace std {
    template<>
    struct hash<Type> {
        inline size_t operator()(const Type& type) const {
            std::hash<TypeKind> hasher;
            return hash_combine(hasher(type.t_kind), type.size_of);
        }
    };

    template<>
    struct hash<TwoType> {
        inline size_t operator()(const TwoType& type) const {
            std::hash<Type> hasher;
            return hash_combine(hasher(type.lhs), type.rhs);
        }
    };
}

// Generates a type from a literal
Type gen_const_type(Tokenizer& tokens);
// Generates the result type from 2 types
Type expl_cast(const Type& lhs, const Type& rhs);
// Generates a type from a explicit type token like float
Type gen_expl_type(Tokenizer& tokens);
// Converts a string float to a hexadecimal floats
std::string strfloat_to_hexfloat(const std::string& str, Type type);

// Converts a type to a string
extern std::unordered_map<Type, std::string> type_to_il_str;
