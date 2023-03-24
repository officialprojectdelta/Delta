#pragma once

#include "node/node.h"
#include "type.h"

// std
#include <unordered_map>
#include <list>

// A entry in the symtables (declaration or definition)
// Will have other modifies at some point
struct GlobalEntry
{
    Type type;
    std::string name;
    bool defined;
};

// A entry for a function symtable
struct FuncEntry
{
    Type type;
    std::string name;
    bool defined;
    std::list<ArgNode> args;
};

// Need to store definitions of functions and globals
extern std::unordered_map<std::string, FuncEntry> function_definitions;
extern std::unordered_map<std::string, GlobalEntry> global_definitions;

// Generate the symtables
void generate_symtables(Node* node);