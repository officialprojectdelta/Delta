#include "codegen.h"

#include <sstream>
#include <array>
#include <unordered_map>
#include <optional>
#include <algorithm>

#include "error/error.h"

static std::string text;
static std::string bss;
static std::string data;
static std::string output;

static size_t conditionLblCtr = 0;

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

void cgfPrologue()
{
    oprintf(&text, "    pushq %rbp\n");
    oprintf(&text, "    movq %rsp, %rbp\n");
}

void cgfEpilogue()
{
    oprintf(&text, "    movq %rbp, %rsp\n");
    oprintf(&text, "    popq %rbp\n");
}

std::string varLoc(Node* node, Symtable& symtable)
{
    std::stringstream varLoc;
    if (node->loc == Location::LOCAL)
    {
        varLoc << "-" << symtable.locals[node->varName].loc << "(%rbp)";
        return varLoc.str();
    }
    else if (node->fnName != "")
    {
        for (auto arg : symtable.globals[node->fnName].args)
        {
            if (arg.name == node->varName) 
            {
                varLoc << arg.loc << "(%rbp)";
                return varLoc.str();
            }
        }

        throw compiler_error("Thats bad");
    }
    else 
    {
        varLoc << node->varName << "(%rip)";
        return varLoc.str();
    }
}

// Forward decls
void cgExp(Node* node, const std::string& loc, bool reg, Symtable& symtable);
void cgStmtExp(Node* node, Symtable& symtable, size_t idx = 0);
void cgStmt(Node* node, Symtable& symtable, std::optional<size_t> end, std::optional<size_t> cont);

void cgExp(Node* node, const std::string& loc, bool reg, Symtable& symtable)
{
    switch (node->kind)
    {
        case NodeKind::NOT:
        {
            cgExp(&node->forward[0], loc, true, symtable);
            
            // Memory support is later
            oprintf(&text, "    cmp $0, %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");
            oprintf(&text, "    sete %", loc, "l\n");

            return;
        }

        case NodeKind::NEG:
        {
            cgExp(&node->forward[0], loc, true, symtable);

            // Memory support is later
            oprintf(&text, "    neg %e", loc, "x\n");

            return;
        }

        case NodeKind::BITCOMPL:
        {
            cgExp(&node->forward[0], loc, true, symtable);

            // Memory support is later
            oprintf(&text, "    not %e", loc, "x\n");

            return;
        }

        case NodeKind::NUM:
        {
            // Memory support is later, as well as default numbers (putting them directly into instructions)
            oprintf(&text, "    movl $", node->tok.value, ", %e", loc, "x\n");

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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "   pop %r", l2, "x\n");
                    oprintf(&text, "   addl %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf(&text, "    addl ", op.str(), ", %e", loc, "x\n");

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
                oprintf(&text, "    subl $", node->forward[1].tok.value, ", %e", loc, "x\n");
                return;
            }
            else if (node->forward[1].kind == NodeKind::VAR)
            {
                oprintf(&text, "    subl ", varLoc(&node->forward[1], symtable), ", %e", loc, "x\n");
                return;
            }

            // Else run like normal

            // Push onto stack cause im to lazy to figure out register allocation
            oprintf(&text, "    push %r", loc, "x\n");

            cgExp(&node->forward[1], "c", true, symtable);

            // Do subtract instruction on %eax, %ecx
            oprintf(&text, "    pop %rax\n");

            if (loc == "a") 
            {
                oprintf(&text, "    subl %ecx, %e", loc, "x\n");
            }
            else
            {
                oprintf(&text, "    subl %eax, %e", loc, "x\n");
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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "   pop %r", l2, "x\n");
                    oprintf(&text, "   imul %e", l2, "x, %e", loc, "x\n");

                    return;
                }
            }

            oprintf(&text, "    imul ", op.str(), ", %e", loc, "x\n");

            return;
        }

        case NodeKind::DIV:
        {
            // Gen left recursive
            cgExp(&node->forward[0], loc, true, symtable);

            std::stringstream op;

            if (node->forward[1].kind == NodeKind::VAR)
            {
                op << varLoc(&node->forward[1], symtable);
            }
            else
            {
                // Push onto stack cause im to lazy to figure out register allocation
                oprintf(&text, "    push %r", loc, "x\n");

                cgExp(&node->forward[1], "c", true, symtable);

                // Pop from stack into %eax
                oprintf(&text, "    pop %rax\n");

                op << "%ecx";
            }
            
            // Sign extend %eax into %edx
            oprintf(&text, "    cdq\n");

            // Do division instruction on %eax, %ecx
            oprintf(&text, "    idivl ", op.str(), "\n");

            if (loc != "a") oprintf(&text, "   movl %eax, %e", loc, "x\n");

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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %eax, %ecx\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    sete %", loc, "l\n");
                    return;
                }
            }

            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");
            oprintf(&text, "    sete %", loc, "l\n");

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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %eax, %ecx\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    setne %", loc, "l\n");
                    return;
                }
            }

            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");
            oprintf(&text, "    setne %", loc, "l\n");

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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    setl %", loc, "l\n");
                    return;
                }
            }
 
            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf(&text, "    setl %", loc, "l\n");
            }
            else
            {
                oprintf(&text, "    setg %", loc, "l\n");
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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    setle %", loc, "l\n");
                    return;
                }
            }
 
            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf(&text, "    setle %", loc, "l\n");
            }
            else
            {
                oprintf(&text, "    setge %", loc, "l\n");
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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    setg %", loc, "l\n");
                    return;
                }
            }
 
            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf(&text, "    setg %", loc, "l\n");
            }
            else
            {
                oprintf(&text, "    setl %", loc, "l\n");
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
                op << varLoc(&node->forward[0], symtable);
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
                    op << varLoc(&node->forward[1], symtable);
                    num = 1;
                }
                else 
                {
                    // Push onto stack cause im to lazy to figure out register allocation
                    oprintf(&text, "    push %r", loc, "x\n");
                    cgExp(&node->forward[1], loc, true, symtable);

                    // Pop from stack into %ecx
                    oprintf(&text, "    pop %r", l2, "x\n");
                    oprintf(&text, "    cmpl %e", loc, "x, %e", l2, "x\n");
                    oprintf(&text, "    mov $0, %e", loc, "x\n");
                    oprintf(&text, "    setge %", loc, "l\n");
                    return;
                }
            }
 
            oprintf(&text, "    cmpl ", op.str(), ", %e", loc, "x\n");
            oprintf(&text, "    mov $0, %e", loc, "x\n");

            // Hack because of stupid operand order
            if (num) 
            {
                oprintf(&text, "    setge %", loc, "l\n");
            }
            else
            {
                oprintf(&text, "    setle %", loc, "l\n");
            }

            return; 
        }

        case NodeKind::OR:
        {
            // Gen left recursive
            std::stringstream op1;
            if (node->forward[0].kind == NodeKind::VAR)
            {
                op1 << varLoc(&node->forward[0], symtable);
            }
            else
            {
                cgExp(&node->forward[0], loc, true, symtable);
                op1 << "%e" << loc << "x";
            }

            oprintf(&text, "    cmpl $0, ", op1.str(), "\n");
            oprintf(&text, "    je .LC", conditionLblCtr, "\n");
            size_t c2 = conditionLblCtr;
            conditionLblCtr++;
            oprintf(&text, "    mov $1, %e", loc, "x\n");
            oprintf(&text, "    jmp .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;

            // C2 lable (evaluate both exprs)
            oprintf(&text, ".LC", c2, ":\n");

            // Check other side of operation if first side doesn't provide enough info
            std::stringstream op2;
            if (node->forward[1].kind == NodeKind::VAR)
            {
                op2 << varLoc(&node->forward[1], symtable);
            }
            else
            {
                cgExp(&node->forward[1], loc, true, symtable);
                op2 << "%e" << loc << "x";
            }
            
            // Set value
            oprintf(&text, "    cmpl $0, ", op2.str(), "\n");
            oprintf(&text, "    movl $0, %e", loc, "x\n");
            oprintf(&text, "    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(&text, ".LC", end, ":\n");

            return; 
        }

        case NodeKind::AND:
        {
            // Gen left recursive
            std::stringstream op1;
            if (node->forward[0].kind == NodeKind::VAR)
            {
                op1 << varLoc(&node->forward[1], symtable);
            }
            else
            {
                cgExp(&node->forward[0], loc, true, symtable);
                op1 << "%e" << loc << "x";
            }

            oprintf(&text, "    cmpl $0, ", op1.str(), "\n");
            oprintf(&text, "    je .LC", conditionLblCtr, "\n");
            size_t end = conditionLblCtr;
            conditionLblCtr++;
            
            // Check other side of operation if first side doesn't provide enough info
            std::stringstream op2;
            if (node->forward[1].kind == NodeKind::VAR)
            {
                op2 << varLoc(&node->forward[1], symtable);
            }
            else
            {
                cgExp(&node->forward[1], loc, true, symtable);
                op2 << "%e" << loc << "x";
            }
            
            // Set value
            oprintf(&text, "    cmpl $0, ", op2.str(), "\n");
            oprintf(&text, "    movl $0, %e", loc, "x\n");
            oprintf(&text, "    setne %", loc, "l\n");

            // The lable that, if the upper statement evaluates as true, will jump too
            oprintf(&text, ".LC", end, ":\n");

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
                firstHalf << varLoc(&node->forward[0], symtable);
            }
            else 
            {
                cgExp(&node->forward[1], loc, true, symtable);
                firstHalf << "%e" << loc << "x";
            }

            oprintf(&text, "    movl ", firstHalf.str(), ", ", varLoc(&node->forward[0], symtable), "\n");

            return;
        }

        case NodeKind::POSTFIXINC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf(&text, "    movl ",  varLoc(&node->forward[0], symtable), ", %e", loc, "x\n");
            oprintf(&text, "    incl ",  varLoc(&node->forward[0], symtable), "\n");        
            return;
        }

        case NodeKind::POSTFIXDEC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf(&text, "    movl ",  varLoc(&node->forward[0], symtable), ", %e", loc, "x\n");
            oprintf(&text, "    decl ",  varLoc(&node->forward[0], symtable), "\n");        
            return;
        }

        case NodeKind::PREFIXINC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf(&text, "    incl ",  varLoc(&node->forward[0], symtable), "\n");
            oprintf(&text, "    movl ",  varLoc(&node->forward[0], symtable), ", %e", loc, "x\n");
            return;
        }

        case NodeKind::PREFIXDEC:
        {
            // Make sure lhs is assignable and exists
            if (node->forward[0].kind != NodeKind::VAR) throw compiler_error("Expr is not assignable");

            oprintf(&text, "    decl ",  varLoc(&node->forward[0], symtable), "\n");
            oprintf(&text, "    movl ",  varLoc(&node->forward[0], symtable), ", %e", loc, "x\n");

            return;
        }

        case NodeKind::VAR:
        {
            oprintf(&text, "    movl ",  varLoc(node, symtable), ", %e", loc, "x\n");

            return;
        }

        case NodeKind::FUNCALL:
        {
            for (size_t i = node->forward.size(); i > 0; i--)
            {
                cgStmtExp(&node->forward[i - 1], symtable);

                oprintf(&text, "    pushq %rax\n");
            }

            // Function call
            oprintf(&text, "    call ", node->tok.value, "\n");

            if (node->forward.size()) oprintf(&text, "    add $", 8 * node->forward.size(), ", %rsp\n");
            return;
        }

        default:
        {
            throw compiler_error("Bad nodetype %d", (size_t) node->kind);
        }
    }
}

void cgStmtExp(Node* node, Symtable& symtable, size_t idx)
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
            firstHalf << varLoc(&node->forward[0], symtable);
        }
        else 
        {
            cgExp(&node->forward[idx], "a", true, symtable);
            firstHalf << "%eax";
        }
        
        oprintf(&text, "    movl ", firstHalf.str(), ", ", varLoc(node, symtable), "\n");
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
            oprintf(&text, "    ret\n");
            return;
        }
        case NodeKind::IF:
        {
            cgStmtExp(node, symtable);

            oprintf(&text, "    cmpl $0, %eax\n");
            oprintf(&text, "    je .LC", conditionLblCtr, "\n");

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
                if (node->forward.size()) cgStmtExp(&node->forward[0], symtable);
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

                oprintf(&text, "    cmpl $0, %eax\n");
                oprintf(&text, "    je .LC", end, "\n");
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

            oprintf(&text, "    cmpl $0, %eax\n");
            oprintf(&text, "    je .LC", end, "\n");

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
            oprintf(&text, "    cmpl $0, %eax\n");
            oprintf(&text, "    je .LC", top, "\n");
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
            
            oprintf(&text, ".globl ", node->forward[i].tok.value, "\n");
            oprintf(&text, node->forward[i].tok.value, ":\n");

            cgfPrologue();

            bool returned = 0;

            for (size_t j = 0; j < node->forward[i].forward.size(); j++)
            {
                if (node->forward[i].forward.size() && node->forward[i].forward[j].kind != NodeKind::ARG)
                {
                    if (node->forward[i].forward[j].kind == NodeKind::RETURN) returned = 1;
                    cgStmt(&node->forward[i].forward[j], symtable, std::nullopt, std::nullopt);            
                }
            }

            if (!returned)
            {
                oprintf(&text, "    movl $0, %eax\n");
                cgfEpilogue();
                oprintf(&text, "    ret\n");
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
                if (node->forward[i].forward[0].kind == NodeKind::NUM)
                {
                    if (node->forward[i].forward[0].tok.value == "0")
                    {
                        oprintf(&bss, ".globl ", node->forward[i].varName, "\n");
                        oprintf(&bss, node->forward[i].varName, ":\n");
                        oprintf(&bss, "    .zero 8\n\n");
                    }
                    else
                    {
                        oprintf(&data, ".globl ", node->forward[i].varName, "\n");
                        oprintf(&data, node->forward[i].varName, ":\n");
                        oprintf(&data, "    .quad ", node->forward[i].forward[0].tok.value, "\n\n");
                    }
                }
                else throw compiler_error("Expected a constant as global variable initializer");
            }
            else 
            {
                oprintf(&bss, ".globl ", node->forward[i].varName, "\n");
                oprintf(&bss, node->forward[i].varName, ":\n");
                oprintf(&bss, "    .zero 8\n\n");
            }
        }
        else 
        {
            throw compiler_error("Expected a valid global scope statement");
        }
    }

    oprintf(&output, ".text\n");
    oprintf(&output, text);

    oprintf(&output, ".align 8\n");
    oprintf(&output, ".bss\n");
    oprintf(&output, bss);

    oprintf(&output, ".align 8\n");
    oprintf(&output, ".data\n");
    oprintf(&output, data);

    return output;
}
