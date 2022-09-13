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
            // Infinite allocation of registers

            //     add
            // sub      div
            // 8, add  40, 5
            //   4, 3

            // if right is a number (checked before right recursive call)
            // then right is directly put into the call (if it can be) otherwise 
            // it is recursivly called

            // register allocator does 2 things
            // checks for forced usecases (such as division and multiplication) 
            // if there are 2 registers left, than (still incrementing the rcounter)
            // pushes the highest used register
            // and pops it when the register count falls low enough

            // mov %r0, 4
            // add %r0, 3

            // mov %r1, 4
            // add %r1, 3

            // sub %r0, %r1

            // Force usecase will change upper registers

            // xor %edx, %edx
            // mov %eax, 40
            // mov %r1, 5
            // idiv %r1

            // add %r0, %r0

            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // Pop from stack into %ecx
            oprintf("    pop %rcx\n");

            // Do add instruction on %eax, %ecx
            if (loc == "a") 
            {
                oprintf("    pop %rcx\n");
                oprintf("    addl %ecx, %e", loc, "x\n");
            }
            else
            {
                oprintf("    pop %rax\n");
                oprintf("    addl %eax, %e", loc, "x\n");
            }

            return;
        }

        case NodeKind::SUB:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

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
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            if (loc == "a") 
            {
                oprintf("    pop %rcx\n");
                oprintf("    imul %ecx, %e", loc, "x\n");
            }
            else
            {
                oprintf("    pop %rax\n");
                oprintf("    imul %eax, %e", loc, "x\n");
            }            

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

            return;
        }

        case NodeKind::EQ:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Equality follows the communitive property
            oprintf("    cmpl %eax, %ecx\n");
            oprintf("    mov $0, %eax\n");
            oprintf("    sete %", loc, "l\n");

            return; 
        }

        case NodeKind::NOTEQ:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Equality follows the communitive property
            oprintf("    cmpl %eax, %ecx\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setne %", loc, "l\n");

            return; 
        }

        case NodeKind::LESS:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Comparison doesn't follow the communitive property
            oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setl %", loc, "l\n");

            return; 
        }

        case NodeKind::LESSEQ:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Comparison doesn't follow the communitive property
            oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setle %", loc, "l\n");

            return; 
        }

        case NodeKind::GREATER:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Comparison doesn't follow the communitive property
            oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setg %", loc, "l\n");

            return; 
        }

        case NodeKind::GREATEREQ:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true);

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf("    push %r", loc, "x\n");

            cgExp(&node->forward[1], loc, true);

            // VERY HACKY CODE
            // Pop from stack into %ecx
            std::string l2 = loc == "a" ? "c" : "a";
 
            oprintf("    pop %r", l2, "x\n");
            
            // Comparison doesn't follow the communitive property
            oprintf("    cmpl %e", loc, "x, %e", l2, "x\n");
            oprintf("    mov $0, %e", loc, "x\n");
            oprintf("    setge %", loc, "l\n");

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
