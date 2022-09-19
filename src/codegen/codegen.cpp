#include "codegen.h"

#include <sstream>
#include <array>
#include <unordered_map>
#include <optional>

#include "error/error.h"

static std::string output;
static size_t conditionLblCtr = 0;

template <typename Ty>
void oprintf(Ty arg1)
{
    std::stringstream str;
    str << arg1;
    output.append(str.str());
    return;
}

template <typename Ty, typename... types>
void oprintf(Ty arg1, types... args)
{
    std::stringstream str;
    str << arg1;
    output.append(str.str());
    oprintf(args...);
    return;
}

void cgfPrologue()
{
    oprintf("    pushq %rbp\n");
    oprintf("    movq %rsp, %rbp\n");
}

void cgfEpilogue()
{
    oprintf("    movq %rbp, %rsp\n");
    oprintf("    popq %rbp\n");
}

void cgExp(Node* node, const std::string& loc, bool reg, Symtable& symtable)
{
    switch (node->kind)
    {
        case NodeKind::NOT:
        {
            cgExp(&node->forward[0], loc, true, symtable);
            
            // Memory support is later
            oprintf("    cmp $0, %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    sete %", loc, "l\n");

            return;
        }

        case NodeKind::NEG:
        {
            cgExp(&node->forward[0], loc, true, symtable);

            // Memory support is later
            oprintf("    neg %e", loc, "x\n");

            return;
        }

        case NodeKind::BITCOMPL:
        {
            cgExp(&node->forward[0], loc, true, symtable);

            // Memory support is later
            oprintf("    not %e", loc, "x\n");

            return;
        }

        case NodeKind::NUM:
        {
            // Memory support is later, as well as default numbers (putting them directly into instructions)
            oprintf("    movl $", node->tok.value, ", %e", loc, "x\n");

            return;
        }

        case NodeKind::ADD:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;

            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("   pop %r", l2, "x\n");
                    oprintf("   addl %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf("    addl ", op.str(), ", %e", loc, "x\n");

            return;
        }

        case NodeKind::SUB:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true, symtable);

            // Only look if right subnode is a number/var
            // Because of operand ordering
            if (node->forward[1].kind == NodeKind::NUM)
            {
                oprintf("    subl $", node->forward[1].tok.value, ", %e", loc, "x\n");
                return;
            }
            else if (node->forward[1].kind == NodeKind::VAR)
            {
                oprintf("    subl -", symtable.locals[node->forward[1].varName].loc, "(%rbp), %e", loc, "x\n");
                return;
            }

            // Else run like normal

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], "c", true, symtable);

            // Do subtract instruction on %eax, %ecx
            oprintf("    pop %rax\n");

            if (loc == "a") 
            {
                oprintf("    subl %ecx, %e", loc, "x\n");
            }
            else
            {
                oprintf("    subl %eax, %e", loc, "x\n");
            }  

            return;
        }

        case NodeKind::MUL:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;

            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("   pop %r", l2, "x\n");
                    oprintf("   imul %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf("    imul ", op.str(), ", %e", loc, "x\n");

            return;
        }

        case NodeKind::DIV:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true, symtable);

            std::stringstream op;

            if (node->forward[1].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
            }
            else
            {
                // Push onto stack cause im to lazy to figure out register allocation
                oprintf("    push %r", loc, "x\n");

                cgExp(&node->forward[1], "c", true, symtable);

                // Pop from stack into %eax
                oprintf("    pop %rax\n");

                op << "%ecx";
            }
            
            // Sign extend %eax into %edx
            oprintf("    cdq\n");

            // Do division instruction on %eax, %ecx
            oprintf("    idivl ", op.str(), "\n");

            if (loc != "a") oprintf("   movl %eax, %e", loc, "x\n");

            return;  
        }

        case NodeKind::EQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;

            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %eax, %ecx\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    sete %", loc, "l\n");
                    return;
                }
            }

            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    sete %", loc, "l\n");

            return;
        }

        case NodeKind::NOTEQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;

            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %eax, %ecx\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setne %", loc, "l\n");
                    return;
                }
            }

            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            return;
        }

        case NodeKind::LESS:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;
            bool num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                    num = 1;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setl %", loc, "l\n");
                    return;
                }
            }
 
            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf("    setl %", loc, "l\n");
            }
            else
            {
                oprintf("    setg %", loc, "l\n");
            }

            return; 
        }

        case NodeKind::LESSEQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;
            bool num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                    num = 1;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setle %", loc, "l\n");
                    return;
                }
            }
 
            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf("    setle %", loc, "l\n");
            }
            else
            {
                oprintf("    setge %", loc, "l\n");
            }

            return; 
        }

        case NodeKind::GREATER:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;
            bool num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                    num = 1;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setg %", loc, "l\n");
                    return;
                }
            }
 
            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf("    setg %", loc, "l\n");
            }
            else
            {
                oprintf("    setl %", loc, "l\n");
            }

            return; 
        }

        case NodeKind::GREATEREQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            std::stringstream op;
            bool num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                op << "$" << node->forward[0].tok.value;
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else if (node->forward[0].kind == NodeKind::VAR)
            {
                op << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
                cgExp(&node->forward[1], loc, true, symtable);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true, symtable);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    op << "$" << node->forward[1].tok.value;
                    num = 1;
                }
                else if (node->forward[1].kind == NodeKind::VAR)
                {
                    op << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setge %", loc, "l\n");
                    return;
                }
            }
 
            oprintf("    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf("    setge %", loc, "l\n");
            }
            else
            {
                oprintf("    setle %", loc, "l\n");
            }

            return; 
        }

        case NodeKind::OR:
        {
            // Gen left recursive
            std::stringstream op1;
            if (node->forward[0].kind == NodeKind::VAR)
            {
                op1 << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
            }
            else
            {
                cgExp(&node->forward[0], loc, true, symtable);
                op1 << "%e" << loc << "x";
            }

            oprintf("    cmpl $0, ", op1.str(), "\n");
            oprintf("    je .LC", conditionLblCtr, "\n");
            size_t c2 = conditionLblCtr;
            conditionLblCtr++;
            oprintf("    mov $1, %e", loc, "x\n");
            oprintf("    jmp .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // C2 lable (evaluate both exprs)
            oprintf(".LC", c2, ":\n");

            // Check other side of operation if first side doesn't provide enough info
            std::stringstream op2;
            if (node->forward[1].kind == NodeKind::VAR)
            {
                op2 << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
            }
            else
            {
                cgExp(&node->forward[1], loc, true, symtable);
                op2 << "%e" << loc << "x";
            }
            
            // Set value
            oprintf("    cmpl $0, ", op2.str(), "\n");
            oprintf("    movl $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(".LC", end, ":\n");

            return; 
        }

        case NodeKind::AND:
        {
            // Gen left recursive
            std::stringstream op1;
            if (node->forward[0].kind == NodeKind::VAR)
            {
                op1 << "-" << symtable.locals[node->forward[0].varName].loc << "(%rbp)";
            }
            else
            {
                cgExp(&node->forward[0], loc, true, symtable);
                op1 << "%e" << loc << "x";
            }

            oprintf("    cmpl $0, ", op1.str(), "\n");
            oprintf("    je .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;
            
            // Check other side of operation if first side doesn't provide enough info
            std::stringstream op2;
            if (node->forward[1].kind == NodeKind::VAR)
            {
                op2 << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
            }
            else
            {
                cgExp(&node->forward[1], loc, true, symtable);
                op2 << "%e" << loc << "x";
            }
            
            // Set value
            oprintf("    cmpl $0, ", op2.str(), "\n");
            oprintf("    movl $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(".LC", end, ":\n");

            return; 
        }

        case NodeKind::ASSIGN:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            // What the variable is being assigned to
            std::stringstream firstHalf;

            if (node->forward[1].kind == NodeKind::NUM) 
            {
                firstHalf << "$" << node->forward[1].tok.value;
            }
            else if (node->forward[1].kind == NodeKind::VAR)
            {
                firstHalf << "-" << symtable.locals[node->forward[1].varName].loc << "(%rbp)";
            }
            else 
            {
                cgExp(&node->forward[1], loc, true, symtable);
                firstHalf << "%e" << loc << "x";
            }

            oprintf("    movl ", firstHalf.str(), ", -", symtable.locals[node->forward[0].varName].loc, "(%rbp)\n");

            return;
        }

        case NodeKind::POSTFIXINC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf("    movl -", symtable.locals[node->forward[0].varName].loc, "(%rbp), %e", loc, "x\n");
            oprintf("    incl -", symtable.locals[node->forward[0].varName].loc, "(%rbp)\n");
            return;
        }

        case NodeKind::POSTFIXDEC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");


            oprintf("    movl -", symtable.locals[node->forward[0].varName].loc, "(%rbp), %e", loc, "x\n");
            oprintf("    decl -", symtable.locals[node->forward[0].varName].loc, "(%rbp)\n");
            return;
        }

        case NodeKind::PREFIXINC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf("    incl -", symtable.locals[node->forward[0].varName].loc, "(%rbp)\n");
            oprintf("    movl -", symtable.locals[node->forward[0].varName].loc, "(%rbp), %e", loc, "x\n");

            return;
        }

        case NodeKind::PREFIXDEC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf("    decl -", symtable.locals[node->forward[0].varName].loc, "(%rbp)\n");
            oprintf("    movl -", symtable.locals[node->forward[0].varName].loc, "(%rbp), %e", loc, "x\n");
            
            return;
        }

        case NodeKind::VAR:
        {
            oprintf("    movl -", symtable.locals[node->varName].loc, "(%rbp), %e", loc, "x\n");

            return;
        }

        default:
        {
            throw compiler_error("Bad nodetype %d", (size_t) node->kind);
        }
    }
}

void cgStmtExp(Node* node, Symtable& symtable, size_t idx = 0)
{
    // The final location that the expression will boil down to
    // For return statements, it is rax/eax
    // For variable assignments, it is the stack location of the variable

    if (node->kind == NodeKind::RETURN) 
    {
        // The actual return type will be found in the declaration table
        cgExp(&node->forward[idx], "a", true, symtable);
    }
    else if (node->kind == NodeKind::IF) 
    {
        // The actual return type will be found in the declaration table
        cgExp(&node->forward[idx], "a", true, symtable);
    }
    else if (node->kind == NodeKind::DECL)
    {
        // What the variable is being assigned to
        std::stringstream firstHalf;

        if (node->forward[idx].kind == NodeKind::NUM) 
        {
            firstHalf << "$" << node->forward[idx].tok.value;
        }
        else if (node->forward[idx].kind == NodeKind::VAR)
        {
            firstHalf << "-" << symtable.locals[node->forward[idx].varName].loc << "(%rbp)\n";
        }
        else 
        {
            cgExp(&node->forward[idx], "a", true, symtable);
            firstHalf << "%eax";
        }
        
        oprintf("    movl ", firstHalf.str(), ", -", symtable.locals[node->varName].loc, "(%rbp)\n");
    }
    else if (node->kind == NodeKind::NOEXPR) {}
    else
    {   
        cgExp(node, "a", true, symtable);
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
            cgStmtExp(node, symtable);
            cgfEpilogue();
            oprintf("    ret\n");
            return;
        }
        case NodeKind::IF:
        {
            cgStmtExp(node, symtable);

            oprintf("    cmpl $0, %eax\n");
            oprintf("    je .LC", conditionLblCtr, "\n");

            size_t s2 = conditionLblCtr;
            conditionLblCtr++;

            cgStmt(&node->forward[1], symtable, end, cont);

            // Else statement
            if (node->forward.size() == 3) 
            {
                oprintf("    jmp .LC", conditionLblCtr, "\n");

                size_t end = conditionLblCtr;
                conditionLblCtr++;

                oprintf(".LC", s2, ":\n");

                cgStmt(&node->forward[2], symtable, end, cont);

                oprintf(".LC", end, ":\n");
            }
            else 
            {
                oprintf(".LC", s2, ":\n");
            }

            return;
        }
        case NodeKind::FOR:
        {
            // First part of loop is decl
            if (node->forward[0].kind == NodeKind::DECL)
            {
                if (node->forward.size()) cgStmtExp(&node->forward[0], symtable);
            }
            // First part of loop is expression
            else 
            {
                cgStmtExp(&node->forward[0], symtable);
            }   

            // Lable for evaluating the condition
            oprintf(".LC", conditionLblCtr, ":\n");
            size_t condEval = conditionLblCtr;
            conditionLblCtr++;

            // Evaluate condition, jump to end if false 
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // Use control statement, if it is NULLEXPR print no code
            if (node->forward[1].kind != NodeKind::NOEXPR)
            {
                cgStmtExp(&node->forward[1], symtable);

                oprintf("    cmpl $0, %eax\n");
                oprintf("    je .LC", end, "\n");
            } // else creates infinite loop, no code is printed

            size_t cont = conditionLblCtr;
            conditionLblCtr++;

            // Body of for loop
            cgStmt(&node->forward[3], symtable, end, cont);

            // Lable for continue is here
            oprintf(".LC", cont, ":\n");

            cgStmtExp(&node->forward[2], symtable);

            // Go back to top of loop, and end of loop lable
            oprintf("    jmp .LC", condEval, "\n");
            oprintf(".LC", end, ":\n");

            return;
        }   
        case NodeKind::WHILE:
        {
            // Lable for evaluating the condition (start of loop)
            oprintf(".LC", conditionLblCtr, ":\n");
            size_t condEval = conditionLblCtr;
            conditionLblCtr++;

            // Evaluate condition, jump to end if false 
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            cgStmtExp(&node->forward[0], symtable);

            oprintf("    cmpl $0, %eax\n");
            oprintf("    je .LC", end, "\n");

            // Body of while loop
            cgStmt(&node->forward[1], symtable, end, condEval);

            // Go back to top of loop, and end of loop lable
            oprintf("    jmp .LC", condEval, "\n");
            oprintf(".LC", end, ":\n");

            return;
        }
        case NodeKind::DO:
        {
            // Lable for statement (start of loop)
            oprintf(".LC", conditionLblCtr, ":\n");
            size_t top = conditionLblCtr;
            conditionLblCtr++;

            size_t cont = conditionLblCtr;
            conditionLblCtr++;

            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // Body of while loop
            cgStmt(&node->forward[0], symtable, end, cont);

            cgStmtExp(&node->forward[1], symtable);

            oprintf(".LC", cont, ":\n");
            oprintf("    cmpl $0, %eax\n");
            oprintf("    je .LC", top, "\n");
            oprintf(".LC", end, ":\n");

            return;
        }
        case NodeKind::BREAK:
        {
            if (end.has_value())
            {
                oprintf("    jmp .LC", end.value(), "\n");
            }

            return;
        }
        case NodeKind::CONTINUE:
        {
            if (cont.has_value())
            {
                oprintf("    jmp .LC", cont.value(), "\n");
            }

            return;
        }
        case NodeKind::DECL:
        {
            if (node->forward.size()) cgStmtExp(node, symtable);
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
        }
    }
}

std::string& codegen(Node* node, Symtable& symtable)
{
    if (node->kind != NodeKind::PRGRM) throw std::runtime_error("Invalid parser");

    node = &node->forward[0];

    if (node->kind != NodeKind::FUNCTION) throw std::runtime_error("Expected a definition");

    oprintf(".globl ", node->tok.value, "\n");
    oprintf(node->tok.value, ":\n");

    cgfPrologue();

    bool returned = 0;

    for (size_t i = 0; i < node->forward.size(); i++)
    {
        if (node->forward[i].kind == NodeKind::RETURN) returned = 1;
        cgStmt(&node->forward[i], symtable, std::nullopt, std::nullopt);
    }

    if (!returned)
    {
        oprintf("    movl $0, %eax\n");
        cgfEpilogue();
        oprintf("    ret\n");
    }

    return output;
}
