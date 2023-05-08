#include "symt.h"

std::unordered_map<std::string, FuncEntry> function_definitions;
std::unordered_map<std::string, GlobalEntry> global_definitions;

void generate_symtables(Node* node)
{
    node->visit_symt();
}

void FunctionNode::visit_symt()
{
    if (function_definitions.contains(this->name.value) && function_definitions[this->name.value].defined)
    {
        if (this->defined) throw compiler_error("Redefinition of function %s\n", this->name.value.c_str());
        else return;
    }
    std::unordered_map<std::string, size_t> arg_to_il_name;
    size_t j = 0;
    for (auto i = std::begin(this->args); i != std::end(this->args); i++, j++)
    {
        if (arg_to_il_name.contains((*i).tok.value)) throw compiler_error("Redefinition of argument %s\n", (*i).tok.value.c_str());    
        arg_to_il_name[(*i).tok.value] = j;
    } 
    function_definitions[this->name.value] = {this->type, this->name.value, this->defined, this->args, std::move(arg_to_il_name), j};
}

void DeclNode::visit_symt()
{
    if (global_definitions.contains(this->name.value) && global_definitions[this->name.value].defined) 
    {
        if (this->defined) throw compiler_error("Redefinition of global variable %s\n", this->name.value.c_str());
        else return;
    }
    global_definitions[this->name.value] = {this->type, this->name.value, this->defined};
}