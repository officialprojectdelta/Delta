#include "type.h"


// std
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

std::unordered_map<Type, std::string> type_to_il_str({
    {{TypeKind::INT, 8}, "i64"},
    {{TypeKind::INT, 4}, "i32"},
    {{TypeKind::INT, 2}, "i16"},
    {{TypeKind::INT, 1}, "i8"},
    {{TypeKind::UNSIGNED, 8}, "i64"},
    {{TypeKind::UNSIGNED, 4}, "i32"},
    {{TypeKind::UNSIGNED, 2}, "i16"},
    {{TypeKind::UNSIGNED, 1}, "i8"},
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
Type gen_expl_type(Tokenizer& tokens, Type type)
{
    std::unordered_map<TokenType, Type> type_map({
        {TokenType::TLONG, {TypeKind::INT, 8}},
        {TokenType::TINT, {TypeKind::INT, 4}},
        {TokenType::TSHORT, {TypeKind::INT, 2}},
        {TokenType::TCHAR, {TypeKind::INT, 1}},
        {TokenType::TFLOAT, {TypeKind::FLOAT, 4}},
        {TokenType::TDOUBLE, {TypeKind::FLOAT, 8}},
    });

    std::unordered_set<Type> type_set({
        {TypeKind::INT, 8},
        {TypeKind::INT, 4},
        {TypeKind::INT, 2},
        {TypeKind::INT, 1},
        {TypeKind::FLOAT, 4},
        {TypeKind::FLOAT, 8},
    });

    if (type_map.contains(tokens.cur().type))
    {
        if (type_set.contains(type)) throw compiler_error("Invalid type for %s", type_to_il_str[type].c_str());
        
        tokens.inc();
        
        if (type.t_kind == TypeKind::UNSIGNED && type_map[tokens.prev().type].t_kind == TypeKind::INT) 
        {
            type.t_kind = TypeKind::UNSIGNED;
            type.size = type_map[tokens.prev().type].size;
            return gen_expl_type(tokens, type); 
        }
        else if (type.t_kind == TypeKind::UNSIGNED) throw compiler_error("Invalid type for unsigned %s", type_to_il_str[type].c_str());
        else
        {
            type.t_kind = type_map[tokens.prev().type].t_kind;
            type.size = type_map[tokens.prev().type].size;
            return gen_expl_type(tokens, type);
        } 
    }
    else if (tokens.cur().type == TokenType::UNSIGNED)
    {
        tokens.inc();
        if (type != Type{TypeKind::NULLTP, 0}) throw compiler_error("Type %s has already been declared", type_to_il_str[type].c_str());
        else return gen_expl_type(tokens, {TypeKind::UNSIGNED, 0});
    }
    else if (tokens.cur().type == TokenType::MUL && type != Type{TypeKind::NULLTP, 0})
    {
        tokens.inc();
        type.num_pointers++;
        return gen_expl_type(tokens, type);
    }
    else if (tokens.cur().type == TokenType::CONST)
    {
        tokens.inc();
        if (type.size_of() || type.is_const) throw compiler_error("Type %s has already been declared", type_to_il_str[type].c_str());
        type.is_const = true;
        return gen_expl_type(tokens, type);
    }

    else return type;
}

// Does a cast for a binary operation
Type bin_op_cast(const Type& t1, const Type& t2)
{
    // If both types are pointers, or either type is a pointer and the other is a float
    if ((t1.num_pointers && t2.num_pointers) || (t1.num_pointers && t2.t_kind == TypeKind::FLOAT) || (t2.num_pointers && t1.t_kind == TypeKind::FLOAT)) throw compiler_error("Invalid operands for binary expression: '%s' and '%s'", type_to_string(t1).c_str(), type_to_string(t2).c_str());
    if (t1 == t2) return t1;
    
    if (t1.t_kind == TypeKind::FLOAT || t2.t_kind == TypeKind::FLOAT) { return {TypeKind::FLOAT, (t1.size_of() > t2.size_of()) ? t1.size_of() : t2.size_of()};}
    else if (t1.num_pointers) return t1;
    else if (t2.num_pointers) return t2;
    else { return {TypeKind::INT, (t1.size_of() > t2.size_of()) ? t1.size_of() : t2.size_of()};}
}

// Converts a stringfloat to a hexadecimal string
std::string strfloat_to_hexfloat(const std::string& str, Type type)
{
    double value = std::stod(str);
    if (type.size_of() == 4) *(size_t*)&value &= 0xFFFFFFFFE0000000;
    std::stringstream ss;
    ss << "0x" << std::hex << *(size_t*)&value;
    return ss.str();
}

// Converts a type to a string
std::string type_to_string(const Type& type)
{
    return type.num_pointers ? "ptr" : type_to_il_str[type];
}
