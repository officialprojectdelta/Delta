#include "codegen.h"

#include "util.h"

std::string output

void codegen(Node* node)
{
    node->visit(&output);
}

void ProgramNode::visit(std::string* write)
{
    for (auto x = forward.begin(); x != forward.end(); x++)
    {
        (*x)->visit(write);
    }
}

void ArgNode::visit(std::string* write)
{
    sprinta(write, "Arg Type: ", (int) type, " Val: ", tok.value, "\n");
}

void BlockStmtNode::visit(std::string* write)
{
    for (auto x = forward.begin(); x != forward.end(); x++)
    {
        (*x)->visit(write);
    }
}

void FunctionNode::visit(std::string* write)
{
    sprinta(write, "Function Type: ", (int) type, " Name: ", name.value, "\n");

    for (auto x : args)
    {
        x.visit(write);
    }

    statements.visit(write);
}

void NoExpr::visit(std::string* write)
{
    sprinta(write, "Nothing to see\n");
}

void UnaryOpNode::visit(std::string* write)
{
    sprinta(write, "UnaryOp: ", (int) op, "\n");

    forward->visit(write);
}

void BinaryOpNode::visit(std::string* write)
{
    sprinta(write, "BinaryOp: ", (int) op, "\n");

    lhs->visit(write);
    rhs->visit(write);
}

void TernNode::visit(std::string* write)
{
    sprinta(write, "Ternary: \n");

    condition->visit(write);
    lhs->visit(write);
    rhs->visit(write);
}

void LiteralNode::visit(std::string* write)
{
    sprinta(write, "Literal Type: ", (int) type, " Value: ", value.value, "\n");
}

void VarNode::visit(std::string* write)
{
    sprinta(write, "Variable Name: ", name.value, "\n");
}

void FuncallNode::visit(std::string* write)
{
    sprinta(write, "Function Name: ", name.value, "\n");
    
    for (auto x = args.begin(); x != args.end(); x++)
    {
        (*x)->visit(write);
    }
}

void DeclNode::visit(std::string* write)
{
    sprinta(write, "@", this->name.value, " = dso_local global ", type_to_il_str[this->type], " ");

    if (assign) assign->visit(write);
    else if (type.t_kind == TypeKind::FLOAT) sprinta(write, "0.000000e+00");
    else if (type.t_kind == TypeKind::INT) sprinta(write, "0");

    sprinta(write, ", align ", type.size_of, "\n\n");
}

void BreakNode::visit(std::string* write)
{
    sprinta(write, "Break\n");
}

void ContinueNode::visit(std::string* write)
{
    sprinta(write, "Continue\n");
}

void RetNode::visit(std::string* write)
{
    sprinta(write, "Returning\n");

    value->visit(write);
}

void IfNode::visit(std::string* write)
{
    sprinta(write, "If\n");

    condition->visit(write);
    statement->visit(write);
    if (else_stmt) else_stmt->visit(write); 
}

void ForNode::visit(std::string* write)
{
    sprinta(write, "For\n");

    initial->visit(write);
    condition->visit(write);
    end->visit(write);
    statement->visit(write);
}

void WhileNode::visit(std::string* write)
{
    sprinta(write, "While\n");

    condition->visit(write);
    statement->visit(write);
}