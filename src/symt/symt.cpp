#include "symt.h"

#include <sstream>

#include "error/error.h"

// The number of created scopes
size_t scopeCtr = 0;

// The stack idx 
size_t stackCtr = 4;

void incScope(Scope* scope, size_t size)
{
    stackCtr+=size;
    scope->stackOffset+=size;
}

void genScope(Scope* current, Symtable& symtable, Node* node);
void checkVars(Scope* scope, Symtable& symtable, Node* node);

// Goes through an expression recusively and checks if a variable has been used before it has been declared
void checkVars(Scope* scope, Symtable& symtable, Node* node)
{
    if (node->kind == NodeKind::VAR) 
    {
        while (true)
        {
            std::stringstream scopeNm;
            scopeNm << node->tok.value << scope->scopeCtr;

            if (scope->locals.contains(scopeNm.str()))
            {
                node->varName = scopeNm.str();
                break;
            }

            if (scope->back == nullptr)
            {
                // Once globals are added, check for them
                throw compiler_error("Variable '%s' has not been declared", node->tok.value.c_str());
            }

            scope = scope->back;
        }
    }
    else if (node->kind == NodeKind::BLOCKSTMT)
    {
        scopeCtr++;
        scope->forward.emplace_back(scope, scopeCtr);
        genScope(&scope->forward.back(), symtable, node);
        stackCtr -= scope->forward.back().stackOffset;
        return;
    }

    for (size_t i = 0; i < node->forward.size(); i++)
    {
        checkVars(scope, symtable, &node->forward[i]);
    }

    return;
}

// Generate all entries in a scope, and recusively call for new scopes
// Does error handling for variables out of scope
// Once scopes are actually added, recursive aproach will be used
// There will be a recursive data and an actual data
// The actual data contains all the info and is just a list
// The recursive data is recursively stored, and just contains local var names
void genScope(Scope* current, Symtable& symtable, Node* node)
{
    if (node->kind == NodeKind::FUNCTION || node->kind == NodeKind::BLOCKSTMT || node->kind == NodeKind::FOR)
    {
        for (size_t i = 0; i < node->forward.size(); i++)
        {
            if (node->forward[i].kind == NodeKind::DECL)
            {
                // Add declaration to symtable
                // Make sure it isn't already declared

                // The name + scope of variable
                std::stringstream scopeNm;
                scopeNm << node->forward[i].tok.value << current->scopeCtr;
                node->forward[i].varName = scopeNm.str();
                
                // Check if variable has already been declared in this scope
                if (symtable.locals.contains(scopeNm.str())) throw compiler_error("Variable %s already declared", scopeNm.str().c_str());

                // Insert to symtable
                symtable.locals.insert({scopeNm.str(), {node->forward[i].type, stackCtr}});

                // Insert to scope tree
                current->locals.insert(scopeNm.str());

                // Increment stack index (will depend on type once there are multiple)
                incScope(current, 4);

                // Check the assign stmt, if there is one
                if (node->forward[i].forward.size()) 
                {
                    checkVars(current, symtable, &node->forward[i].forward[0]);
                }
            }
            else if (node->forward[i].kind == NodeKind::BLOCKSTMT || node->forward[i].kind == NodeKind::FOR)
            {
                scopeCtr++;
                current->forward.emplace_back(current, scopeCtr);
                genScope(&current->forward.back(), symtable, &node->forward[i]);
                stackCtr -= current->forward.back().stackOffset;
            }
            else { 
                if (node->kind == NodeKind::VAR) 
                {
                    while (true)
                    {
                        if (current == nullptr)
                        {
                            // Once globals are added, check for them
                            throw compiler_error("Variable '%s' has not been declared", node->tok.value.c_str());
                        }

                        std::stringstream scopeNm;
                        scopeNm << node->tok.value << current->scopeCtr;

                        if (current->locals.contains(scopeNm.str()))
                        {
                            node->varName = scopeNm.str();
                            break;
                        }
                        current = current->back;
                    }

                    continue;
                }

                // Check exp
                for (; i < node->forward.size(); i++)
                {
                    checkVars(current, symtable, &node->forward[i]);
                }
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
            // Create function scope
            Scope* fn = new Scope(nullptr, scopeCtr);
            symtable.globals.insert({node->forward[i].tok.value, node->forward[i].type});
            genScope(fn, symtable, &node->forward[i]);
            delete fn;
        }
        else throw compiler_error("%d is not a valid global entry", (int) node->forward[i].kind);
    }

    return symtable;
} 