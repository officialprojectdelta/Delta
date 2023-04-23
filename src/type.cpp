#include "type.h"


// std
#include <array>
#include <unordered_map>
#include <sstream>

std::unordered_map<Type, std::string> type_to_il_str({
    {{TypeKind::INT, 8}, "i64"},
    {{TypeKind::INT, 4}, "i32"},
    {{TypeKind::INT, 2}, "i16"},
    {{TypeKind::INT, 1}, "i8"},
    {{TypeKind::BOOL, 1}, "i1"},
    {{TypeKind::FLOAT, 4}, "float"},
    {{TypeKind::FLOAT, 8}, "double"},
    {{TypeKind::NULLTP, 0}, "null"}
});

// Generates a type based on constants (such as integers, floating points, arrays, and string literals)
Type gen_const_type(Tokenizer& tokens)
{
    switch (tokens.cur().type)
    {
        case TokenType::INTV:
        {
            auto val = std::stol(tokens.cur().value);
            if ((char) val == val) return {TypeKind::INT, 1};
            else if ((short) val == val) return {TypeKind::INT, 2};
            else if ((int) val == val) return {TypeKind::INT, 4};
            else return {TypeKind::INT, 8};
        }
        case TokenType::FLOATV:
        {
            auto val = std::stod(tokens.cur().value);
            if ((float) val == val) return {TypeKind::FLOAT, 4};
            else return {TypeKind::FLOAT, 8};
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
        case TokenType::TLONG:
        {
            tokens.inc();
            return {TypeKind::INT, 8};
        }
        case TokenType::TINT:
        {
            tokens.inc();
            return {TypeKind::INT, 4};
        }
        case TokenType::TSHORT:
        {
            tokens.inc();
            return {TypeKind::INT, 2};
        }
        case TokenType::TCHAR:
        {
            tokens.inc();
            return {TypeKind::INT, 1};
        }
        case TokenType::TFLOAT:
        {
            tokens.inc();
            return {TypeKind::FLOAT, 4};
        }
        case TokenType::TDOUBLE:
        {
            tokens.inc();
            return {TypeKind::FLOAT, 8};
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

// Converts a stringfloat to a hexadecimal string
std::string strfloat_to_hexfloat(const std::string& str, Type type)
{
    double value = std::stod(str);
    if (type.size_of == 4) *(size_t*)&value &= 0xFFFFFFFFE0000000;
    std::stringstream sstr;
    sstr << "0x" << std::hex << *(size_t*)&value;
    return sstr.str();
}