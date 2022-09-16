#pragma once

#include <unordered_map>

#include "parser/node.h"

// Symtable impl
// For globals, go through list (only one for now) and write down info
// Then for functions/structs go through and write down local/member data
// For each new scope entry, (only for functions), change the scope, reset the counter, and continue

// Info for the locals in the symtable
struct localInfo
{
    Type type;
    std::string scopenm;
    size_t loc;
};

// The symbol table, containing the globals and the locals
struct Symtable
{
    std::unordered_map<std::string, Type> globals;
    std::unordered_map<std::string, localInfo> locals;
};

// Generate a symtable from a AST
Symtable genEntries(Node* node);