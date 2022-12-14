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

void genFn(Scope* current, Symtable& symtable, Node* node);
void genScope(Scope* current, Symtable& symtable, Node* node, globalInfo fnArgs);
void checkVars(Scope* scope, Symtable& symtable, Node* node, globalInfo fnArgs);

// Goes through an expression recusively and checks if a variable has been used before it has been declared
void checkVars(Scope* scope, Symtable& symtable, Node* node, globalInfo fnArgs)
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
                node->loc = Location::LOCAL;
                break;
            }

            if (scope->back == nullptr)
            {
                // Once globals are added, check for them
                // Check function varibles, then globals

                std::stringstream str;

                bool found = 0;
                for (auto arg : fnArgs.args)
                {
                    if (arg.name == node->tok.value)
                    {
                        node->fnName = fnArgs.fnName;
                        // node->fnName.append("fun");
                        node->loc = Location::FUNCTION;
                        node->varName = node->tok.value;
                        found = 1;
                        break;
                    }
                }
                
                if (found) break;
                
                str.str(std::string());

                str << node->tok.value << "glb";

                if (symtable.globals.contains(str.str()) && symtable.globals[str.str()].kind == SKind::GLOBAL)
                {
                    node->varName = str.str();
                    node->loc = Location::GLOBAL;
                    break;
                }

                throw compiler_error("Variable '%s' has not been declared", node->tok.value.c_str());
            }

            scope = scope->back;
        }

        return;
    }
    else if (node->kind == NodeKind::BLOCKSTMT || node->kind == NodeKind::FOR)
    {
        scopeCtr++;
        scope->forward.emplace_back(scope, scopeCtr);
        genScope(&scope->forward.back(), symtable, node, fnArgs);
        stackCtr -= scope->forward.back().stackOffset;
        return;
    }
    else if (node->kind == NodeKind::FUNCALL)
    {
        // Verifiy function call
        // In the future, this will actually deduce types, so it needs to be done first
        for (size_t i = 0; i < node->forward.size(); i++)
        {
            checkVars(scope, symtable, &node->forward[i], fnArgs);
        }

        if (!symtable.globals.contains(node->tok.value)) throw compiler_error("Function has not been declared yet");

        for (size_t i = 0; ; i++)
        {
            // DO TYPECHECKING LATER
            if (symtable.globals[node->tok.value].args.size() == i && node->forward.size() == i) break;
            else if (symtable.globals[node->tok.value].args.size() == i || node->forward.size() == i) throw compiler_error("Invalid function call");
        }

        return; 
    }

    for (size_t i = 0; i < node->forward.size(); i++)
    {   
        checkVars(scope, symtable, &node->forward[i], fnArgs);
    }

    return;
}

void genFn(Symtable& symtable, Node* node)
{
    if (!symtable.globals.contains(node->tok.value))
    {   
        symtable.globals.insert({node->tok.value, globalInfo()});
        symtable.globals[node->tok.value].kind = SKind::FUNCTION;
        symtable.globals[node->tok.value].type = node->type;
        symtable.globals[node->tok.value].fnName = node->tok.value;

        // Is definition
        if (node->forward.size())
        {
            if (node->forward.back().kind != NodeKind::ARG) 
            {
                // Only add variable names on a definition
                // Add var locations
                symtable.globals[node->tok.value].define = true;

                // Add var locations & types & names
                size_t locCtr = 16;
                size_t i = 0;
                for (; node->forward[i].kind == NodeKind::ARG; i++)
                {
                    symtable.globals[node->tok.value].args.emplace_back();
                    symtable.globals[node->tok.value].args[i].name = node->forward[i].tok.value;
                    symtable.globals[node->tok.value].args[i].type = node->forward[i].type;
                    symtable.globals[node->tok.value].args[i].loc = locCtr + i * 8;

                    for (size_t j = 0; j < symtable.globals[node->tok.value].args.size(); j++)
                    {
                        if (j == i) break;

                        if (symtable.globals[node->tok.value].args[j].name == node->forward[i].tok.value) throw compiler_error("Redefinition of function argument %s", node->forward[i].tok.value.c_str());
                    }
                } 

                Scope* scope = new Scope(nullptr, scopeCtr);
                genScope(scope, symtable, &node->forward.back(), symtable.globals[node->tok.value]);
                stackCtr -= scope->stackOffset;
                scopeCtr++;
                delete scope;
            }
            // Is declaration
            else
            {   
                symtable.globals[node->tok.value].define = false;

                // Add var locations & types
                size_t locCtr = 16;
                size_t i = 0;
                for (size_t i = 0; i < node->forward.size(); i++)
                {
                    symtable.globals[node->tok.value].args.emplace_back();
                    symtable.globals[node->tok.value].args[i].name = "";
                    symtable.globals[node->tok.value].args[i].type = node->forward[i].type;
                    symtable.globals[node->tok.value].args[i].loc = locCtr + i * 8;
                }
            }
        }
        else 
        {
            symtable.globals[node->tok.value].define = false;
        }
    }
    // Check for different decls and redefs
    else 
    {
        // First check for different declarations
        for (size_t i = 0; i < node->forward.size(); i++)
        {
            if (node->forward[i].kind != NodeKind::ARG)
            {
                if (symtable.globals[node->tok.value].args.size() != i) throw compiler_error("conflicting types for %s", node->tok.value.c_str());
                break;
            }

            if (node->forward[i].type != symtable.globals[node->tok.value].args[i].type) throw compiler_error("conflicting types for %s", node->tok.value.c_str());
        }

        if (node->forward.back().kind != NodeKind::ARG)
        {
            // Check for already defined
            if (symtable.globals[node->tok.value].define) throw compiler_error("redefinition of function %s", node->tok.value.c_str());
            symtable.globals[node->tok.value].define = 1;

            // Add var names to function
            // Make sure there are no matches
            size_t i = 0;
            for (; i < node->forward.size(); i++)
            {
                if (node->forward[i].kind != NodeKind::ARG) break;

                symtable.globals[node->tok.value].args[i].name = node->forward[i].tok.value;
                // Check for var name matches

                for (size_t j = 0; j < symtable.globals[node->tok.value].args.size(); j++)
                {
                    if (j == i) break;

                    if (symtable.globals[node->tok.value].args[j].name == node->forward[i].tok.value) throw compiler_error("Redefinition of function argument %s", node->forward[i].tok.value.c_str());
                }
            }

            Scope* scope = new Scope(nullptr, scopeCtr);
            genScope(scope, symtable, &node->forward.back(), symtable.globals[node->tok.value]);
            stackCtr -= scope->stackOffset;
            scopeCtr++;
            delete scope;

            return;
        } 
    }        

    return;
}

// Generate all entries in a scope, and recusively call for new scopes
// Takes in a current scope for recursive calling, the output symtable reference
// The current node, and the function args for var matching
void genScope(Scope* current, Symtable& symtable, Node* node, globalInfo fnArgs)
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
                node->forward[i].loc = Location::LOCAL;
                
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
                    checkVars(current, symtable, &node->forward[i].forward[0], fnArgs);
                }
            }
            else
            {       
                checkVars(current, symtable, &node->forward[i], fnArgs);
            }
        }
    }
    else throw compiler_error("Not a valid scope for the symtable");

    return;
}

Symtable genEntries(Node* node)
{
    Symtable symtable;
    for (size_t i = 0; i < node->forward.size(); i++)
    {
        if (node->forward[i].kind == NodeKind::FUNCTION)
        {
            genFn(symtable, &node->forward[i]);
        }
        else if (node->forward[i].kind == NodeKind::DECL)
        {
            // Add to globals
            // Subnode isn't checked because it cannot contain variables anyways (feature added later?)]

            std::stringstream varNm;
            varNm << node->forward[i].tok.value << "glb";

            if (symtable.globals.contains(varNm.str())) throw compiler_error("Variable %s already declared", node->forward[i].tok.value.c_str());

            symtable.globals.insert({varNm.str(), {SKind::GLOBAL, node->forward[i].type}});
            node->forward[i].varName = varNm.str();
        }
        else throw compiler_error("%d is not a valid global entry", (int) node->forward[i].kind);
    }

    return symtable;
} 