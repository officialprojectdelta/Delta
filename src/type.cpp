#include "type.h"

#include <array>
#include <unordered_map>

std::unordered_map<Type, std::string> type_to_string({
    {{TypeKind::INT, 4}, "i32"},
    {{TypeKind::FLOAT, 4}, "f32"},
    {{TypeKind::NULLTP, 0}, "null"}
});

// Generates a type based on constants (such as integers, floating points, arrays, and string literals)
Type gen_const_type(Tokenizer& tokens)
{
    switch (tokens.cur().type)
    {
        // Later size checking will be done here
        case TokenType::INTV:
        {
            return {TypeKind::INT, 4};
        }
        case TokenType::FLOATV:
        {
            return {TypeKind::FLOAT, 4};
        }
        default:
        {
            return {TypeKind::NULLTP};
        }
    }
}

// Generates an explicit type (ie from a variable declaration or a cast)
Type gen_expl_type(Tokenizer& tokens)
{
    switch (tokens.cur().type)
    {
        case TokenType::TCHAR:
        {
            tokens.inc();
            return {TypeKind::INT, 1};
        }
        case TokenType::TINT:
        {
            tokens.inc();
            return {TypeKind::INT, 4};
        }
        case TokenType::TFLOAT:
        {
            tokens.inc();
            return {TypeKind::FLOAT, 4};
        }
        default:
        {
            tokens.inc();
            return {TypeKind::NULLTP};
        }
    }
}

// Does an explicit cast
Type expl_cast(const Type& t1, const Type& t2)
{
    if (t1 == t2) return t1;
    
    if (t1.t_kind == TypeKind::FLOAT || t2.t_kind == TypeKind::FLOAT) { return {TypeKind::FLOAT, (t1.size_of > t2.size_of) ? t1.size_of : t2.size_of};}
    else { return {TypeKind::INT, (t1.size_of > t2.size_of) ? t1.size_of : t2.size_of};}
}

std::string to_string(Type type)
{
    return type_to_string[type];
}