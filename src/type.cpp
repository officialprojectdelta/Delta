#include "type.h"

#include <array>
#include <unordered_set>

// Generates a type based on constants (such as integers, floating points, arrays, and string literals)
Type genConstType(Tokenizer& tokens)
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

// Checks if right type is castable to left type
// Returns notype if not
Type implCastable(const Type& lhs, const Type& rhs)
{
    // Make sure it isn't a narrow
    if (rhs.size_of > lhs.size_of) return {TypeKind::NULLTP};
    
    // Check if identical
    if (lhs == rhs) return lhs;

    if (lhs.tKind == TypeKind::FLOAT)
    {
        if (rhs.tKind == TypeKind::FLOAT)
        {
            return lhs;
        }
        else if (rhs.tKind == TypeKind::INT)
        {
            return rhs;
        }
    }
    else if (lhs.tKind == TypeKind::INT)
    {
        if (rhs.tKind == TypeKind::FLOAT)
        {
            return lhs;
        }
        else if (rhs.tKind == TypeKind::INT)
        {
            return rhs;
        }
    }

    return lhs;
}

// Does implicit casting of types on an expression (takes 2 input types)
// Returns notype if not a match
Type exprCast(const Type& lhs, const Type& rhs)
{
    // Check if identical
    if (lhs == rhs) return lhs;

    if (lhs.tKind == TypeKind::FLOAT)
    {
        if (rhs.tKind == TypeKind::FLOAT)
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
        else if (rhs.tKind == TypeKind::INT)
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
    else if (lhs.tKind == TypeKind::INT)
    {
        if (rhs.tKind == TypeKind::FLOAT)
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
        else if (rhs.tKind == TypeKind::INT)
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
Type genExplType(Tokenizer& tokens)
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