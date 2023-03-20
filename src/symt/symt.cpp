// #include "symt.h"

// #include <sstream>
// #include <unordered_set>

// #include "error/error.h"

// std::unordered_set<NodeKind> binOp
// {
//     NodeKind::ADD, 
//     NodeKind::SUB,
//     NodeKind::MUL, 
//     NodeKind::DIV, 
//     NodeKind::MOD,
// };

// std::unordered_set<NodeKind> boolbinOp
// {
//     NodeKind::EQ,
//     NodeKind::NOTEQ,
//     NodeKind::LESS,
//     NodeKind::LESSEQ,
//     NodeKind::GREATER,
//     NodeKind::GREATEREQ,
// };

// std::unordered_set<NodeKind> unaryOp
// {
//     NodeKind::NOT, 
//     NodeKind::NEG, 
//     NodeKind::BITCOMPL,
//     NodeKind::PREFIXINC,
//     NodeKind::PREFIXDEC,
//     NodeKind::POSTFIXINC,
//     NodeKind::POSTFIXDEC,
// };

// // The number of created scopes
// size_t scopeCtr = 0;

// // The stack idx 
// size_t stackCtr = 4;

// void incScope(Scope* scope, size_t size)
// {
//     stackCtr+=size;
//     scope->stackOffset+=size;
// }

// void genFn(Scope* current, Symtable& symtable, Node* node);
// void genScope(Scope* current, Symtable& symtable, Node* node, globalInfo fnArgs);
// void checkVars(Scope* scope, Symtable& symtable, Node* node, globalInfo fnArgs);

// // Goes through an node recusively and checks if a variable has been used before it has been declared
// void checkVars(Scope* scope, Symtable& symtable, Node* node, globalInfo fnArgs)
// {
//     if (node->kind == NodeKind::VAR) 
//     {
//         while (true)
//         {
//             std::stringstream scopeNm;
//             scopeNm << node->tok.value << scope->scopeCtr;

//             if (scope->locals.contains(scopeNm.str()))
//             {
//                 node->varName = scopeNm.str();
//                 node->loc = Location::LOCAL;
//                 node->type = symtable.locals[scopeNm.str()].type;
//                 break;
//             }

//             if (scope->back == nullptr)
//             {
//                 // Once globals are added, check for them
//                 // Check function varibles, then globals

//                 std::stringstream str;

//                 str << fnArgs.fnName << node->tok.value << "fvar";

//                 bool found = 0;
//                 for (auto arg : fnArgs.args)
//                 {
//                     if (arg.name == node->tok.value)
//                     {
//                         node->fnName = fnArgs.fnName;
//                         node->loc = Location::FUNCTION;
//                         node->varName = str.str();
//                         node->type = arg.type;
//                         found = 1;
//                         break;
//                     }
//                 }
                
//                 if (found) break;
                
//                 str.str(std::string());

//                 str << node->tok.value << "glb";

//                 if (symtable.globals.contains(str.str()) && symtable.globals[str.str()].kind == SKind::GLOBAL)
//                 {
//                     node->varName = str.str();
//                     node->loc = Location::GLOBAL;
//                     node->type = symtable.globals[str.str()].type;
//                     break;
//                 }

//                 throw compiler_error("Variable '%s' has not been declared", node->tok.value.c_str());
//             }

//             scope = scope->back;
//         }

//         return;
//     }
//     else if (node->kind == NodeKind::BLOCKSTMT || node->kind == NodeKind::FOR)
//     {
//         scopeCtr++;
//         scope->forward.emplace_back(scope, scopeCtr);
//         genScope(&scope->forward.back(), symtable, node, fnArgs);
//         stackCtr -= scope->forward.back().stackOffset;
//         return;
//     }
//     else if (node->kind == NodeKind::FUNCALL)
//     {
//         // Verifiy function call
//         // In the future, this will actually deduce types, so it needs to be done first
//         for (size_t i = 0; i < node->forward.size(); i++)
//         {
//             checkVars(scope, symtable, &node->forward[i], fnArgs);
//         }

//         std::stringstream fnName;
//         fnName << node->tok.value << "fun";

//         if (!symtable.globals.contains(fnName.str())) 
//         {
//             std::cout << fnName.str() << std::endl;
//             throw compiler_error("Function has not been declared yet");
//         }

//         for (size_t i = 0; ; i++)
//         {
//             if (node->forward.size() == 0 && symtable.globals[fnName.str()].args.size() == 0) break;

//             if (symtable.globals[fnName.str()].args.size() - 1 == i && node->forward.size() - 1 == i) 
//             {   
//                 if (!implCastable(symtable.globals[fnName.str()].args[i].type, node->forward[i].type)) throw compiler_error("Type %llu is not castable to type %llu", (size_t) node->forward[i].type.tKind, (size_t) symtable.globals[fnName.str()].args[i].type.tKind);
//                 break;
//             }

//             if (!implCastable(symtable.globals[fnName.str()].args[i].type, node->forward[i].type)) throw compiler_error("Type %llu is not castable to type %llu", (size_t) node->forward[i].type.tKind, (size_t) symtable.globals[fnName.str()].args[i].type.tKind);
//             if (symtable.globals[fnName.str()].args.size() == i || node->forward.size() == i) throw compiler_error("Invalid function call");
//         }

//         node->type = symtable.globals[fnName.str()].type;
//         node->fnName = fnName.str();

//         return; 
//     }
//     else if (binOp.contains(node->kind))
//     {
//         // Make sure binary op works with both args (get output type of the operation)
//         checkVars(scope, symtable, &node->forward[0], fnArgs);
//         checkVars(scope, symtable, &node->forward[1], fnArgs);

//         Type type = exprCast(node->forward[0].type, node->forward[1].type);
//         if (!type) throw compiler_error("Operator %llu has invalid operands of type %llu and %llu", (size_t) node->kind, (size_t) node->forward[0].type.tKind, (size_t) node->forward[1].type.tKind);
//         node->type = type;

//         return;
//     }
//     else if (boolbinOp.contains(node->kind))
//     {
//         checkVars(scope, symtable, &node->forward[0], fnArgs);
//         checkVars(scope, symtable, &node->forward[1], fnArgs);

//         Type type = exprCast(node->forward[0].type, node->forward[1].type);
//         if (!type) throw compiler_error("Operator %llu has invalid operands of type %llu and %llu", (size_t) node->kind, (size_t) node->forward[0].type.tKind, (size_t) node->forward[1].type.tKind);
//         node->type = {TypeKind::INT, 4};
//     }
//     else if (unaryOp.contains(node->kind))
//     {
//         // Make sure unary op works on the arg 
//         checkVars(scope, symtable, &node->forward[0], fnArgs);

//         node->type = node->forward[0].type;

//         return;
//     }
//     else if (node->kind == NodeKind::ASSIGN)
//     {
//         // Make sure right type is castable to left type
//         checkVars(scope, symtable, &node->forward[0], fnArgs);
//         checkVars(scope, symtable, &node->forward[1], fnArgs);

//         if (!implCastable(node->forward[0].type, node->forward[1].type)) throw compiler_error("Type %llu is not castable to %llu", node->forward[1].type.tKind, node->forward[0].type.tKind);
//         node->type = node->forward[0].type;

//         return;
//     }
//     else if (node->kind == NodeKind::AND || node->kind == NodeKind::OR)
//     {
//         // Make sure both types are convertable to bool (int for now)
//         checkVars(scope, symtable, &node->forward[0], fnArgs);
//         checkVars(scope, symtable, &node->forward[1], fnArgs);

//         if (!implCastable({TypeKind::INT, 4}, node->forward[0].type)) throw compiler_error("Type %llu is not castable to boolean value", (size_t) node->forward[0].type.tKind);
//         if (!implCastable({TypeKind::INT, 4}, node->forward[1].type)) throw compiler_error("Type %llu is not castable to boolean value", (size_t) node->forward[1].type.tKind);
//         node->type = {TypeKind::INT, 4};
        
//         return;
//     }
//     else if (node->kind == NodeKind::RETURN)
//     {
//         // Make sure return type matches function return type
//         checkVars(scope, symtable, &node->forward[0], fnArgs);

//         if (!implCastable(fnArgs.type, node->forward[0].type)) throw compiler_error("Type %llu does not match the return type of function %s", (size_t) node->forward[0].type, fnArgs.fnName.c_str());
//         node->type = fnArgs.type;

//         return;
//     }
//     else if (node->kind == NodeKind::TERN)
//     {
//         // Make sure e1 is castable to bool
//         // Get exprCast of e2 and e3
//         // Set this type to that

//         checkVars(scope, symtable, &node->forward[0], fnArgs);
//         checkVars(scope, symtable, &node->forward[1], fnArgs);
//         checkVars(scope, symtable, &node->forward[2], fnArgs);

//         if (!implCastable({TypeKind::INT, 4}, node->forward[0].type)) throw compiler_error("Type %llu is not castable to boolean value", (size_t) node->forward[0].type.tKind);
//         Type type = exprCast(node->forward[1].type, node->forward[2].type);
//         if (!type) throw compiler_error("Operator %llu has invalid operands of type %llu and %llu", (size_t) node->kind, (size_t) node->forward[1].type.tKind, (size_t) node->forward[2].type.tKind);
//         node->type = type;

//         return;
//     }

//     for (size_t i = 0; i < node->forward.size(); i++)
//     {   
//         checkVars(scope, symtable, &node->forward[i], fnArgs);
//     }

//     return;
// }

// void genFn(Symtable& symtable, Node* node)
// {
//     std::stringstream fnName;

//     fnName << node->tok.value << "fun";

//     node->fnName = fnName.str();

//     if (!symtable.globals.contains(fnName.str()))
//     {   
//         symtable.globals.insert({fnName.str(), globalInfo()});
//         symtable.globals[fnName.str()].kind = SKind::FUNCTION;
//         symtable.globals[fnName.str()].type = node->type;
//         symtable.globals[fnName.str()].fnName = fnName.str();

//         // Is definition
//         if (node->forward.size())
//         {
//             if (node->forward.back().kind != NodeKind::ARG) 
//             {
//                 // Only add variable names on a definition
//                 // Add var locations
//                 symtable.globals[fnName.str()].define = true;

//                 size_t i = 0;
//                 // Add var locations & types & names
//                 for (; node->forward[i].kind == NodeKind::ARG; i++)
//                 {
//                     symtable.globals[fnName.str()].args.emplace_back();
//                     symtable.globals[fnName.str()].args[i].name = node->forward[i].tok.value;
//                     symtable.globals[fnName.str()].args[i].type = node->forward[i].type;

//                     std::stringstream str;

//                     str << fnName.str() << node->forward[i].tok.value << "fvar";
//                     node->forward[i].varName = str.str();

//                     for (size_t j = 0; j < symtable.globals[fnName.str()].args.size(); j++)
//                     {
//                         if (j == i) break;

//                         if (symtable.globals[fnName.str()].args[j].name == node->forward[i].tok.value) throw compiler_error("Redefinition of function argument %s", node->forward[i].tok.value.c_str());
//                     }
//                 } 

//                 Scope* scope = new Scope(nullptr, scopeCtr);
//                 genScope(scope, symtable, &node->forward.back(), symtable.globals[fnName.str()]);
//                 stackCtr -= scope->stackOffset;
//                 scopeCtr++;
//                 delete scope;
//             }
//             // Is declaration
//             else
//             {   
//                 symtable.globals[fnName.str()].define = false;

//                 // Add var locations & types
//                 size_t i = 0;
//                 for (size_t i = 0; i < node->forward.size(); i++)
//                 {
//                     symtable.globals[fnName.str()].args.emplace_back();
//                     symtable.globals[fnName.str()].args[i].name = "";
//                     symtable.globals[fnName.str()].args[i].type = node->forward[i].type;
//                 }
//             }
//         }
//         else 
//         {
//             symtable.globals[fnName.str()].define = false;
//         }
//     }
//     // Check for different decls and redefs
//     else 
//     {
//         // First check for different declarations
//         for (size_t i = 0; i < node->forward.size(); i++)
//         {
//             if (node->forward[i].kind != NodeKind::ARG)
//             {
//                 if (symtable.globals[fnName.str()].args.size() != i) throw compiler_error("conflicting types for %s", node->tok.value.c_str());
//                 break;
//             }

//             if (node->forward[i].type != symtable.globals[fnName.str()].args[i].type) throw compiler_error("conflicting types for %s", fnName.str().c_str());
//         }

//         if (node->forward.back().kind != NodeKind::ARG)
//         {
//             // Check for already defined
//             if (symtable.globals[fnName.str()].define) throw compiler_error("redefinition of function %s", fnName.str().c_str());
//             symtable.globals[fnName.str()].define = 1;

//             // Add var names to function
//             // Make sure there are no matches
//             size_t i = 0;
//             for (; i < node->forward.size(); i++)
//             {
//                 if (node->forward[i].kind != NodeKind::ARG) break;

//                 symtable.globals[fnName.str()].args[i].name = node->forward[i].tok.value;
                
//                 std::stringstream str;
//                 str << fnName.str() << node->forward[i].tok.value << "fvar";
//                 node->forward[i].varName = str.str();
//                 // Check for var name matches

//                 for (size_t j = 0; j < symtable.globals[fnName.str()].args.size(); j++)
//                 {
//                     if (j == i) break;

//                     if (symtable.globals[fnName.str()].args[j].name == node->forward[i].tok.value) throw compiler_error("Redefinition of function argument %s", node->forward[i].tok.value.c_str());
//                 }
//             }

//             Scope* scope = new Scope(nullptr, scopeCtr);
//             genScope(scope, symtable, &node->forward.back(), symtable.globals[fnName.str()]);
//             stackCtr -= scope->stackOffset;
//             scopeCtr++;
//             delete scope;

//             return;
//         } 
//     }        

//     return;
// }

// // Generate all entries in a scope, and recusively call for new scopes
// // Takes in a current scope for recursive calling, the output symtable reference
// // The current node, and the function args for var matching
// void genScope(Scope* current, Symtable& symtable, Node* node, globalInfo fnArgs)
// {
//     if (node->kind == NodeKind::FUNCTION || node->kind == NodeKind::BLOCKSTMT || node->kind == NodeKind::FOR)
//     {
//         for (size_t i = 0; i < node->forward.size(); i++)
//         {
//             if (node->forward[i].kind == NodeKind::DECL)
//             {
//                 // Add declaration to symtable
//                 // Make sure it isn't already declared

//                 // The name + scope of variable
//                 std::stringstream scopeNm;
//                 scopeNm << node->forward[i].tok.value << current->scopeCtr;
//                 node->forward[i].varName = scopeNm.str();
//                 node->forward[i].loc = Location::LOCAL;
                
//                 // Check if variable has already been declared in this scope
//                 if (symtable.locals.contains(scopeNm.str())) throw compiler_error("Variable %s already declared", scopeNm.str().c_str());

//                 // Insert to symtable
//                 symtable.locals.insert({scopeNm.str(), {node->forward[i].type, stackCtr}});

//                 // Insert to scope tree
//                 current->locals.insert(scopeNm.str());

//                 // Increment stack index (will depend on type once there are multiple)
//                 incScope(current, 4);

//                 // Check the assign stmt, if there is one
//                 if (node->forward[i].forward.size()) 
//                 {
//                     checkVars(current, symtable, &node->forward[i].forward[0], fnArgs);
//                 }
//             }
//             else
//             {       
//                 checkVars(current, symtable, &node->forward[i], fnArgs);
//             }
//         }
//     }
//     else throw compiler_error("Not a valid scope for the symtable");

//     return;
// }

// Symtable genEntries(Node* node)
// {
//     Symtable symtable;
//     for (size_t i = 0; i < node->forward.size(); i++)
//     {
//         if (node->forward[i].kind == NodeKind::FUNCTION)
//         {
//             genFn(symtable, &node->forward[i]);
//         }
//         else if (node->forward[i].kind == NodeKind::DECL)
//         {
//             // Add to globals
//             // Subnode isn't checked because it cannot contain variables anyways (feature added later?)]

//             std::stringstream varNm;
//             varNm << node->forward[i].tok.value << "glb";

//             if (symtable.globals.contains(varNm.str())) throw compiler_error("Variable %s already declared", node->forward[i].tok.value.c_str());

//             symtable.globals.insert({varNm.str(), {SKind::GLOBAL, node->forward[i].type}});
//             node->forward[i].varName = varNm.str();
//         }
//         else throw compiler_error("%d is not a valid global entry", (int) node->forward[i].kind);
//     }

//     return symtable;
// } 