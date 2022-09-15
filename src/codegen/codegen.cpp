#include "codegen.h"

#include <sstream>
#include <array>

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

void cgExp(Node* node, const std::string& loc, bool reg)
{
    switch (node->kind)
    {
        case NodeKind::NOT:
        {
            cgExp(&node->forward[0], loc, true);
            
            // Memory support is later
            oprintf("    cmp $0, %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    sete %", loc, "l\n");

            return;
        }

        case NodeKind::NEG:
        {
            cgExp(&node->forward[0], loc, true);

            // Memory support is later
            oprintf("    neg %e", loc, "x\n");

            return;
        }

        case NodeKind::BITCOMPL:
        {
            cgExp(&node->forward[0], loc, true);

            // Memory support is later
            oprintf("    not %e", loc, "x\n");

            return;
        }

        case NodeKind::NUM:
        {
            // Memory support is later, as well as default numbers (putting them directly into instructions)
            oprintf("    mov $", node->tok.value, ", %e", loc, "x\n");

            return;
        }

        case NodeKind::ADD:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("   pop %r", l2, "x\n");
                    oprintf("   addl %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf("    addl $", node->forward[num].tok.value, ", %e", loc, "x\n");

            return;
        }

        case NodeKind::SUB:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Only look if right subnode is a number
            // Because of operand ordering
            if (node->forward[1].kind == NodeKind::NUM)
            {
                oprintf("    subl $", node->forward[1].tok.value, ", %e", loc, "x\n");
                return;
            }

            // Else run like normal

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], "c", true);

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

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("   pop %r", l2, "x\n");
                    oprintf("   imul %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf("    imul $", node->forward[num].tok.value, ", %e", loc, "x\n");

            return;
        }

        case NodeKind::DIV:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], "c", true);

            // Pop from stack into %eax
            oprintf("    pop %rax\n");
            
            // Sign extend %eax into %edx
            oprintf("    cdq\n");

            // Do division instruction on %eax, %ecx
            oprintf("    idiv %ecx\n");

            if (loc != "a") oprintf("   movl %eax, %e", loc, "x\n");

            return;  
        }

        case NodeKind::EQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %eax, %ecx\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    sete %", loc, "l\n");


                    return;
                }
            }

            oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    sete %", loc, "l\n");

            return;
        }

        case NodeKind::NOTEQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %eax, %ecx\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setne %", loc, "l\n");


                    return;
                }
            }

            oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            return;
        }

        case NodeKind::LESS:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setl %", loc, "l\n");

                    return;
                }
            }

            if (num) 
            {
                oprintf("    cmpl %e", loc, "x, $", node->forward[num].tok.value, "\n");
            }
            else 
            {
                oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            }

            oprintf("    mov $0, %e", loc, "x\n");
            // Hack because of stupid operand order
            oprintf("    setg %", loc, "l\n");

            return; 
        }

        case NodeKind::LESSEQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setle %", loc, "l\n");

                    return;
                }
            }

            if (num) 
            {
                oprintf("    cmpl %e", loc, "x, $", node->forward[num].tok.value, "\n");
            }
            else 
            {
                oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            }

            oprintf("    mov $0, %e", loc, "x\n");
            // Hack because of stupid operand order
            oprintf("    setge %", loc, "l\n");

            return; 
        }

        case NodeKind::GREATER:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setg %", loc, "l\n");

                    return;
                }
            }

            if (num) 
            {
                oprintf("    cmpl %e", loc, "x, $", node->forward[num].tok.value, "\n");
            }
            else 
            {
                oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            }

            oprintf("    mov $0, %e", loc, "x\n");
            // Hack because of stupid operand order
            oprintf("    setl %", loc, "l\n");

            return; 
        }

        case NodeKind::GREATEREQ:
        {
            // Gen left recursive
            // Check if number can directly be used in the operation

            std::string l2 = loc == "a" ? "c" : "a";

            char num = 0;
            if (node->forward[0].kind == NodeKind::NUM) 
            {
                num = 0;

                cgExp(&node->forward[1], loc, true);
            }
            else 
            {
                cgExp(&node->forward[0], loc, true);

                if (node->forward[1].kind == NodeKind::NUM) 
                {
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf("    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true);

                    // Pop from stack into %ecx
                    oprintf("    pop %r", l2, "x\n");
                    oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf("    mov $0, %e", loc, "x\n");
                    oprintf("    setge %", loc, "l\n");

                    return;
                }
            }

            if (num) 
            {
                oprintf("    cmpl %e", loc, "x, $", node->forward[num].tok.value, "\n");
            }
            else 
            {
                oprintf("    cmpl $", node->forward[num].tok.value, ", %e", loc, "x\n");
            }

            oprintf("    mov $0, %e", loc, "x\n");
            // Hack because of stupid operand order
            oprintf("    setle %", loc, "l\n");

            return;  
        }

        case NodeKind::OR:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            oprintf("    cmpl $0, %e", loc, "x\n");
            oprintf("    je .LC", conditionLblCtr, "\n");
            size_t c2 = conditionLblCtr;
            conditionLblCtr++;
            oprintf("    mov $1, %e", loc, "x\n");
            oprintf("    jmp .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // C2 lable (evaluate both exprs)
            oprintf("    .LC", c2, ":\n");

            // Check other side of operation if first side doesn't provide enough info

            // Check if right node is a number 
            cgExp(&node->forward[1], loc, true);
            
            // Set value
            oprintf("    cmpl $0, %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(".LC", end, ":\n");

            return; 
        }

        case NodeKind::AND:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            oprintf("    cmpl $0, %e", loc, "x\n");
            oprintf("    je .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;
            
            // Right recurse if the top condition doesn't evaluate 
            cgExp(&node->forward[1], loc, true);
            
            // Set value
            oprintf("    cmpl $0, %e", loc, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(".LC", end, ":\n");

            return; 
        }

        default:
        {
            throw compiler_error("Bad char");
        }
    }
}

void cgStmtExp(Node* node, NodeKind ctx)
{
    // The final location that the expression will boil down to
    // For return statements, it is rax/eax
    // For variable assignments, it is the stack location of the variable

    std::string floc;
    bool reg;
    if (ctx == NodeKind::RETURN) 
    {
        // The actual return type will be found in the declaration table
        floc = "a";
        reg = 1;
    }
    else 
    {
        // Getting the final location for a variable
        throw compiler_error("We don't have variables yet");
    }

    cgExp(node, floc, reg);

    return;
}

void cgStmt(Node* node)
{
    switch (node->kind)
    {
        case NodeKind::RETURN:
        {
            cgStmtExp(&node->forward[0], NodeKind::RETURN);
            oprintf("    ret");
            return;
        }
        
        default:
        {
            throw std::runtime_error("Not a statement");
        }
    }
}

std::string& codegen(Node* node)
{
    if (node->kind != NodeKind::PRGRM) throw std::runtime_error("Invalid parser");

    node = &node->forward[0];

    if (node->kind != NodeKind::FUNCTION) throw std::runtime_error("Expected a definition");

    oprintf(".globl ", node->tok.value, "\n");
    oprintf(node->tok.value, ":\n");

    cgStmt(&node->forward[0]);

    return output;
}
