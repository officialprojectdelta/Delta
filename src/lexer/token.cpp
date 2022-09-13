#include "token.h"

#include <iostream>

// Cout for token
std::ostream& operator<<(std::ostream& os, const Token& t)
{
    if (t.value.size()) return os << (size_t) t.type << ", " << t.value;
    return os << (size_t) t.type;
}