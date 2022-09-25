#pragma once

#include <unordered_map>
#include <unordered_set>

#include "parser/node.h"

// Symtable impl
// For globals, go through list (only one for now) and write down info
// Then for functions/structs go through and write down local/member data
// For each new scope entry, (only for functions), change the scope, reset the counter, and continue

struct Scope 
{
    // Could be unordered map, with type info (once there are multiple types)
    std::unordered_set<std::string> locals;

    // ScopeCtr, the current scope ctr saved when scope is created
    size_t scopeCtr;

    // The amount of bytes stored by this scope, used for deallocation
    size_t stackOffset;

    std::vector<Scope> forward;
    Scope* back;

    Scope(Scope* back, size_t scopeCtr)
    {
        this->back = back;
        this->scopeCtr = scopeCtr;
    }
};

// Info for the locals in the symtable
struct localInfo
{
    Type type;
    size_t loc;
};

enum class SKind
{
    FUNCTION,
    GLOBAL,
    STRUCT,
};

struct arg 
{
    std::string name;
    Type type;
    size_t loc;
};

struct globalInfo
{
    SKind kind;
    Type type;
    bool define;

    // Function only
    std::vector<arg> args;
    std::string fnName;
};

// The symbol table, containing the globals and the locals
struct Symtable
{
    std::unordered_map<std::string, globalInfo> globals;
    std::unordered_map<std::string, localInfo> locals;
};

// Generate a symtable from a AST
Symtable genEntries(Node* node);