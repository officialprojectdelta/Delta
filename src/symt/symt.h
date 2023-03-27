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
    // The type of the function
    Type type;
    // The name
    std::string name;
    // If this is a definition or a declaration 
    bool defined;
    // The list of args
    std::list<ArgNode> args;
    // Convert an arg name to the il temporary value used
    std::unordered_map<std::string, size_t> arg_to_il_name;
    // Highest value reached for temporaries + 1, where the stack/temps can start at
    size_t next_temp;
}

// Need to store definitions of functions and globals
extern std::unordered_map<std::string, FuncEntry> function_definitions;
extern std::unordered_map<std::string, GlobalEntry> global_definitions;

// Generate the symtables
void generate_symtables(Node* node);

// Generate a name from a function
std::string generate_name(Type type, std::string name, const std::list<ArgNode>& args);