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

// Checks if cast of the 2 types
// Returns notype if not
Type impl_cast(const Type& lhs, const Type& rhs)
{
    // Make sure it isn't a narrow
    if (rhs.size_of > lhs.size_of) return {TypeKind::NULLTP};
    
    // Check if identical
    if (lhs == rhs) return lhs;

    if (lhs.t_kind == TypeKind::FLOAT)
    {
        if (rhs.t_kind == TypeKind::FLOAT)
        {
            return lhs;
        }
        else if (rhs.t_kind == TypeKind::INT)
        {
            return rhs;
        }
    }
    else if (lhs.t_kind == TypeKind::INT)
    {
        if (rhs.t_kind == TypeKind::FLOAT)
        {
            return lhs;
        }
        else if (rhs.t_kind == TypeKind::INT)
        {
            return rhs;
        }
    }

    return {TypeKind::NULLTP};
}

// Does implicit casting of types on an expression (takes 2 input types)
// Returns notype if not a match
Type expl_cast(const Type& lhs, const Type& rhs)
{
    // Check if identical
    if (lhs == rhs) return lhs;

    if (lhs.t_kind == TypeKind::FLOAT)
    {
        if (rhs.t_kind == TypeKind::FLOAT)
        {
            if (rhs.size_of > lhs.size_of)
            {
                return rhs;
            }
            else
            {
                return lhs;
            }
        }
        else if (rhs.t_kind == TypeKind::INT)
        {
            if (rhs.size_of > lhs.size_of)
            {
                return {TypeKind::FLOAT, rhs.size_of};
            }
            else
            {
                return lhs;
            }
        }
    }
    else if (lhs.t_kind == TypeKind::INT)
    {
        if (rhs.t_kind == TypeKind::FLOAT)
        {
            if (rhs.size_of > lhs.size_of)
            {
                return rhs;
            }
            else
            {
                return {TypeKind::FLOAT, lhs.size_of};
            }
        }
        else if (rhs.t_kind == TypeKind::INT)
        {
            if (rhs.size_of > lhs.size_of)
            {
                return rhs;
            }
            else
            {
                return lhs;
            }
        }
    }

    return {TypeKind::NULLTP};
}

// Generates an explicit type (ie from a variable declaration or a cast)
Type gen_expl_type(Tokenizer& tokens)
{
    switch (tokens.cur().type)
    {
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

std::string to_string(Type type)
{
    return type_to_string[type];
}