#include "symt.h"

#include "error/error.h"

// Goes through an expression recusively and checks if a variable has been used before it has been declared
void checkExp(std::string scopeName, Symtable& symtable, Node* node)
{
    if (node->kind == NodeKind::VAR && !symtable.locals.contains(node->tok.value)) throw compiler_error("Variable '%s' has not been declared", node->tok.value.c_str());

    for (size_t i = 0; i < node->forward.size(); i++)
    {
        checkExp(scopeName, symtable, &node->forward[i]);
    }

    return;
}

// Generate all entries in a scope, and recusively call for new scopes
// Does error handling for variables out of scope
// Once scopes are actually added, recursive aproach will be used
// There will be a recursive data and an actual data
// The actual data contains all the info and is just a list
// The recursive data is recursively stored, and just contains local var names
void genScope(std::string scopeName, Symtable& symtable, Node* node)
{
    size_t stackCtr = 4;
    if (node->kind == NodeKind::FUNCTION)
    {
        for (size_t i = 0; i < node->forward.size(); i++)
        {
            if (node->forward[i].kind == NodeKind::DECL)
            {
                // Add declaration to symtable
                // Make sure it isn't already declared
                if (symtable.locals.contains(node->forward[i].tok.value))  throw compiler_error("Variable %s already declared", node->forward[i].tok.value.c_str());
                symtable.locals.insert({node->forward[i].tok.value, {node->forward[i].type, scopeName, stackCtr}});
                stackCtr+=4;
                // Check expr
                if (node->forward[i].forward.size()) 
                {
                    checkExp(scopeName, symtable, &node->forward[i].forward[0]);
                }
            }
            else 
            {
                // Check exp
                checkExp(scopeName, symtable, &node->forward[i]);
            }
        }
    }
    else throw compiler_error("Not a valid scope for the symtable");
}

Symtable genEntries(Node* node)
{
    Symtable symtable;
    for (size_t i = 0; i < node->forward.size(); i++)
    {
        if (node->forward[i].kind == NodeKind::FUNCTION)
        {
            symtable.globals.insert({node->forward[i].tok.value, node->forward[i].type});
            genScope(node->forward[i].tok.value, symtable, &node->forward[i]);
        }
        else throw compiler_error("%d is not a valid global entry", (int) node->forward[i].kind);
    }

    return symtable;
} 