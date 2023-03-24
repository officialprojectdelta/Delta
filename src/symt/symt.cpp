#include "symt.h"

std::unordered_map<std::string, FuncEntry> function_definitions;
std::unordered_map<std::string, GlobalEntry> global_definitions;

// Generates a name for a function from the args and the return type/name of function
std::string generate_name(Type type, std::string name, const std::list<ArgNode>& args)
{
    name.append(to_string(type));
    for (auto arg : args)
    {
        name.append(to_string(arg.type));
    }
    return name;
}

void generate_symtables(Node* node)
{
    node->visit_symt();
}

void FunctionNode::visit_symt()
{
    if (function_definitions.contains(generate_name(this->type, this->name.value, this->args)) && function_definitions[generate_name(this->type, this->name.value, this->args)].defined) throw compiler_error("Redefinition of function %s\n", generate_name(this->type, this->name.value, this->args).c_str());
    bool defined = this->statements.forward.size() == 0 ? false : true;
    function_definitions[generate_name(this->type, this->name.value, this->args)] = {this->type, this->name.value, defined, this->args};
}

void DeclNode::visit_symt()
{
    if (global_definitions.contains(this->name.value)) throw compiler_error("Redifinition of global variable %s\n", this->name.value.c_str());
    global_definitions[this->name.value] = {this->type, this->name.value, true};
}