#include "codegen.h"

#include <sstream>
#include <array>
#include <unordered_map>
#include <optional>
#include <algorithm>

#include "error/error.h"

using node_str = std::unordered_map<NodeKind, std::string>;

std::unordered_map<TypeKind, std::string> typeToStr({
    {TypeKind::INT, "i"},
    {TypeKind::FLOAT, "f"},
});

node_str unaryCvt({
    {NodeKind::NOT, "lnot"},
    {NodeKind::NEG, "neg"},
    {NodeKind::BITCOMPL, "not"},
});

node_str binaryCvt({
    {NodeKind::ADD, "add"},
    {NodeKind::SUB, "sub"},
    {NodeKind::MUL, "mul"},
    {NodeKind::DIV, "div"},
    {NodeKind::EQ, "sete"},
    {NodeKind::NOTEQ, "setne"},
    {NodeKind::LESS, "setl"},
    {NodeKind::LESSEQ, "setle"},
    {NodeKind::GREATER, "setg"},
    {NodeKind::GREATEREQ, "setge"},
});

node_str postmod({
    {NodeKind::POSTFIXDEC, "dec"},
    {NodeKind::POSTFIXINC, "inc"},
});

node_str premod({
    {NodeKind::PREFIXDEC, "dec"},
    {NodeKind::PREFIXINC, "inc"},
});

static std::string text;
static std::string data;
static std::string output;

static size_t conditionLblCtr = 0;
static size_t currentTemp = 0;

template <typename Ty>
void oprintf(std::string* write, Ty arg1)
{
    std::stringstream str;
    str << arg1;
    write->append(str.str());
    return;
}

template <typename Ty, typename... types>
void oprintf(std::string* write, Ty arg1, types... args)
{
    std::stringstream str;
    str << arg1;
    write->append(str.str());
    oprintf(write, args...);
    return;
}

std::string varLoc(Node* node, Symtable& symtable)
{
    std::stringstream varLoc;
    varLoc << typeToStr[node->type.tKind] << node->type.size_of * 8 << " &" << node->varName;
    return varLoc.str();
}

std::string constStr(Node* node, Symtable& symtable)
{
    std::stringstream constStr;
    constStr << typeToStr[node->type.tKind] << node->type.size_of * 8 << " " << node->tok.value;
    return constStr.str();
}

std::string tempStr(Node* node)
{
    std::stringstream tempStr;
    tempStr << typeToStr[node->type.tKind] << node->type.size_of * 8 << " %i" << currentTemp;
    return tempStr.str();
}

// Forward decls
void cgExp(Node* node, Symtable& symtable);
void cgStmtExp(Node* node, Symtable& symtable, size_t idx = 0);
void cgStmt(Node* node, Symtable& symtable, std::optional<size_t> end, std::optional<size_t> cont);


void cgExp(Node* node, Symtable& symtable)
{
    if (unaryCvt.contains(node->kind)) 
    {
        cgExp(&node->forward[0], symtable);
        oprintf(&text, "    ", unaryCvt[node->kind], " ", tempStr(node), ", ", tempStr(&node->forward[0]), "\n");
        return;
    }
    else if (postmod.contains(node->kind))
    {
        if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expected variable for operator %ull", (size_t) node->kind);
        oprintf(&text, "    mov ", tempStr(node), ", ", varLoc(&node->forward[0], symtable), "\n");
        oprintf(&text, "    ", postmod[node->kind], varLoc(&node->forward[0], symtable), ", ", varLoc(&node->forward[0], symtable), "\n");
        return;
    }
    else if (premod.contains(node->kind))
    {
        if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expected variable for operator %ull", (size_t) node->kind);
        oprintf(&text, "    ", postmod[node->kind], varLoc(node, symtable), ", ", varLoc(node, symtable), "\n");
        oprintf(&text, "    mov ", tempStr(node), ", ", varLoc(node, symtable), "\n");
        return;
    }
    else if (node->kind == NodeKind::LIT)
    {
        oprintf(&text, "    mov ", tempStr(node), ", ", constStr(node, symtable), "\n");
        return;
    } 
    else if (node->kind == NodeKind::VAR)
    {
        oprintf(&text, "    mov ", tempStr(node), ", ", varLoc(node, symtable), "\n");
        return;
    }
    else if (node->kind == NodeKind::ASSIGN)
    {
        if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expected lvalue for assignment", (size_t) node->kind);
        cgExp(&node->forward[1], symtable);
        oprintf(&text, "    mov ", varLoc(&node->forward[0], symtable), ", ", tempStr(node), "\n");
        return;
    }
    else if (binaryCvt.contains(node->kind))
    {
        cgExp(&node->forward[0], symtable);
        currentTemp++;
        cgExp(&node->forward[1], symtable);
        currentTemp--;
        oprintf(&text, "    ", binaryCvt[node->kind], " ", tempStr(node));
        oprintf(&text, ", ", tempStr(&node->forward[0]));
        currentTemp++;
        oprintf(&text, ", ", tempStr(&node->forward[1]), "\n");
        currentTemp--;
        return;
    }
    else if (node->kind == NodeKind::AND)
    {
        // Check if first half is false
        // If so output 0
        // Otherwise check second half
        // Output second half true/false

        cgExp(&node->forward[0], symtable);
        oprintf(&text, "    setne ", tempStr(node), ", i32 0, ", tempStr(&node->forward[0]), "\n");
        oprintf(&text, "    je .LC", conditionLblCtr, ", i32 0, ", tempStr(node), "\n");
        size_t end = conditionLblCtr;
        conditionLblCtr++;
        cgExp(&node->forward[1], symtable);
        oprintf(&text, "    setne ", tempStr(node), ", i32 0, ", tempStr(&node->forward[1]), "\n");
        oprintf(&text, ".LC", end, ":\n");
        return;
    }
    else if (node->kind == NodeKind::OR)
    {
        cgExp(&node->forward[0], symtable);
        oprintf(&text, "    sete ", tempStr(node), ", i32 0, ", tempStr(&node->forward[0]), "\n");
        oprintf(&text, "    je .LC", conditionLblCtr, ", i32 0, ", tempStr(node), "\n");
        size_t end = conditionLblCtr;
        conditionLblCtr++;
        cgExp(&node->forward[1], symtable);
        oprintf(&text, "    setne ", tempStr(node), ", i32 0, ", tempStr(&node->forward[1]), "\n");
        oprintf(&text, ".LC", end, ":\n");
        return;
    }
    else if (node->kind == NodeKind::FUNCALL)
    {
        oprintf(&text, "    call ", tempStr(node), ", ", node->fnName, "(");
        size_t saveCTemp = currentTemp;
        for (; currentTemp - saveCTemp < node->forward.size(); currentTemp++)
        {
            cgExp(&node->forward[currentTemp - saveCTemp], symtable);
            oprintf(&text, tempStr(&node->forward[currentTemp - saveCTemp]));
            if (node->forward.size() - 1 == currentTemp + saveCTemp) break;
            oprintf(&text, ", ");
        }
        oprintf(&text, ")\n");
        return;
    }

    return;
}

void cgStmtExp(Node* node, Symtable& symtable, size_t idx)
{
    // The final location that the expression will boil down to
    // For return statements, it is rax/eax
    // For variable assignments, it is the stack location of the variable

    if (node->kind == NodeKind::RETURN) 
    {
        // The actual return type will be found in the declaration table
        cgExp(&node->forward[idx], symtable);
    }
    else if (node->kind == NodeKind::IF) 
    {
        cgExp(&node->forward[idx], symtable);
    }
    else if (node->kind == NodeKind::DECL)
    {
        cgExp(&node->forward[idx], symtable);
    }
    else if (node->kind == NodeKind::NOEXPR) {}
    else
    {   
        cgExp(node, symtable);
    }

    return;
}

// Last arguments are for break and continue statements
void cgStmt(Node* node, Symtable& symtable, std::optional<size_t> end, std::optional<size_t> cont)
{
    switch (node->kind)
    {
        case NodeKind::RETURN:
        {
            cgStmtExp(node, symtable, 0);
            oprintf(&text, "    ret ", tempStr(&node->forward[0]), "\n");
            return;
        }
        case NodeKind::IF:
        {
            cgStmtExp(node, symtable, 0);
            oprintf(&text, "    je .LC", conditionLblCtr, " i32 0, ", tempStr(&node->forward[0]), "\n");

            size_t s2 = conditionLblCtr;
            conditionLblCtr++;

            cgStmt(&node->forward[1], symtable, end, cont);

            // Else statement
            if (node->forward.size() == 3) 
            {
                oprintf(&text, "    jmp .LC", conditionLblCtr, "\n");

                size_t end = conditionLblCtr;
                conditionLblCtr++;

                oprintf(&text, ".LC", s2, ":\n");

                cgStmt(&node->forward[2], symtable, end, cont);

                oprintf(&text, ".LC", end, ":\n");
            }
            else 
            {
                oprintf(&text, ".LC", s2, ":\n");
            }

            return;
        }
        case NodeKind::FOR:
        {
            // First part of loop is decl
            if (node->forward[0].kind == NodeKind::DECL)
            {
                if (node->forward.size()) 
                {   
                    cgStmtExp(&node->forward[0], symtable);
                    oprintf(&text, "    def ", varLoc(&node->forward[0], symtable), ", ", tempStr(&node->forward[0]), "\n");
                }
                else
                {   
                    oprintf(&text, "    def ", varLoc(&node->forward[0], symtable), "\n");
                }
            }
            // First part of loop is expression
            else 
            {
                cgStmtExp(&node->forward[0], symtable);
            }   

            // Lable for evaluating the condition
            oprintf(&text, ".LC", conditionLblCtr, ":\n");
            size_t condEval = conditionLblCtr;
            conditionLblCtr++;

            // Evaluate condition, jump to end if false 
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // Use control statement, if it is NULLEXPR print no code
            if (node->forward[1].kind != NodeKind::NOEXPR)
            {
                cgStmtExp(&node->forward[1], symtable);

                oprintf(&text, "    je .LC", end, ", i32 0, ", tempStr(&node->forward[1]), "\n");
            } // else creates infinite loop, no code is printed

            size_t cont = conditionLblCtr;
            conditionLblCtr++;

            // Body of for loop
            cgStmt(&node->forward[3], symtable, end, cont);

            // Lable for continue is here
            oprintf(&text, ".LC", cont, ":\n");

            cgStmtExp(&node->forward[2], symtable);

            // Go back to top of loop, and end of loop lable
            oprintf(&text, "    jmp .LC", condEval, "\n");
            oprintf(&text, ".LC", end, ":\n");

            return;
        }   
        case NodeKind::WHILE:
        {
            // Lable for evaluating the condition (start of loop)
            oprintf(&text, ".LC", conditionLblCtr, ":\n");
            size_t condEval = conditionLblCtr;
            conditionLblCtr++;

            // Evaluate condition, jump to end if false 
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            cgStmtExp(&node->forward[0], symtable);

            oprintf(&text, "    je .LC", end, ", i32 0, ", tempStr(&node->forward[0]), "\n");

            // Body of while loop
            cgStmt(&node->forward[1], symtable, end, condEval);

            // Go back to top of loop, and end of loop lable
            oprintf(&text, "    jmp .LC", condEval, "\n");
            oprintf(&text, ".LC", end, ":\n");

            return;
        }
        case NodeKind::DO:
        {
            // Lable for statement (start of loop)
            oprintf(&text, ".LC", conditionLblCtr, ":\n");
            size_t top = conditionLblCtr;
            conditionLblCtr++;

            size_t cont = conditionLblCtr;
            conditionLblCtr++;

            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // Body of while loop
            cgStmt(&node->forward[0], symtable, end, cont);

            cgStmtExp(&node->forward[1], symtable);

            oprintf(&text, ".LC", cont, ":\n");
            oprintf(&text, "    je .LC", top, ", i32 0, ", tempStr(&node->forward[0]), "\n");
            oprintf(&text, ".LC", end, ":\n");

            return;
        }
        case NodeKind::BREAK:
        {
            if (end.has_value())
            {
                oprintf(&text, "    jmp .LC", end.value(), "\n");
            }

            return;
        }
        case NodeKind::CONTINUE:
        {
            if (cont.has_value())
            {
                oprintf(&text, "    jmp .LC", cont.value(), "\n");
            }

            return;
        }
        case NodeKind::DECL:
        {
            if (node->forward.size()) 
            {   
                cgStmtExp(&node->forward[0], symtable);
                oprintf(&text, "    def ", varLoc(node, symtable), ", ", tempStr(&node->forward[0]), "\n");
            }
            else
            {   
                oprintf(&text, "    def ", varLoc(node, symtable), "\n");
            }
            
            return;
        }
        case NodeKind::BLOCKSTMT:
        {
            for (size_t i = 0; i < node->forward.size(); i++)
            {
                cgStmt(&node->forward[i], symtable, end, cont);
            }

            return;
        }
        default:
        {
            cgStmtExp(node, symtable);
            return;
        }
    }
}

std::string& codegen(Node* node, Symtable& symtable)
{
    if (node->kind != NodeKind::PRGRM) throw std::runtime_error("Invalid parser");

    for (size_t i = 0; i < node->forward.size(); i++)
    {
        if (node->forward[i].kind == NodeKind::FUNCTION) 
        {
            if (node->forward[i].forward.size() && node->forward[i].forward.back().kind == NodeKind::ARG) continue;
            
            oprintf(&text, "global def ", typeToStr[node->forward[i].type.tKind], node->forward[i].type.size_of * 8, " ", node->forward[i].fnName, "(");
            
            // Loop through args and add them to function definition
            for (size_t j = 0; j < node->forward[i].forward.size() - 1; j++)
            {
                if (j != 0) oprintf(&text, ", ");
                oprintf(&text, varLoc(&node->forward[i].forward[j], symtable));
            }

            // Finish function header
            oprintf(&text, "):\n");

            Node* blkstmt = &node->forward[i].forward.back();

            bool returned = 0;
            for (size_t j = 0; j < blkstmt->forward.size(); j++)
            {
                // Actual statement generation
                cgStmt(&blkstmt->forward[j], symtable, std::nullopt, std::nullopt);
                if (blkstmt->forward[j].kind == NodeKind::RETURN) returned = 1;
            }

            if (!returned)
            {
                oprintf(&text, "ret null\n");
            }

            oprintf(&text, "\n");
        } 
        else if (node->forward[i].kind == NodeKind::DECL)
        {
            // Check if it has an initalizer
            // If it does, make sure that it is a constant
            // If the constant is zero skip to it doesn't have an initalizer
            // else the constant is nonzero put it in data section
            // If it isn't a constant error
            // Else put it in bss

            if (node->forward[i].forward.size()) 
            {
                if (node->forward[i].forward[0].kind == NodeKind::LIT)
                {
                    oprintf(&data, "    global def ", varLoc(&node->forward[i], symtable), " ", constStr(&node->forward[i].forward[0], symtable), "\n");
                }
                else throw compiler_error("Expected a constant as global variable initializer");
            }
            else 
            {
                oprintf(&data, "    global def ", varLoc(&node->forward[i], symtable), "\n");
            }
        }
        else 
        {
            throw compiler_error("Expected a valid global scope statement");
        }
    }

    oprintf(&output, ".text\n");
    oprintf(&output, text);

    if (data != "")
    {
        oprintf(&output, ".data\n");
        oprintf(&output, data);
    }

    return output;
}
