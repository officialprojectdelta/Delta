#include "codegen.h"

#include "util.h"
#include "symt/symt.h

// std

// The output string
std::string output;

// The result string, where something would go (so we dont need return values)
std::string result;

// The type of the result 
Type result_type;

// An additional result string for variable to put their actual location
std::string location;

// The stack declared variables and lables, (the vector is for multiple stack frames)
std::vector<std::unordered_map<std::string, size_t>> var_map;
// The next unused temp value, increment after use
size_t next_temp;

std::unordered_map<TypeKind, std::string> after_decimal({
    {TypeKind::FLOAT, ".000000e+00"},
    {TypeKind::INT, ""},
});

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

}

void BlockStmtNode::visit(std::string* write)
{
    var_map.push_back();

    for (auto x = forward.begin(); x != forward.end(); x++)
    {
        (*x)->visit(write);
    }

    var_map.pop_back();
}

void FunctionNode::visit(std::string* write)
{
    // Only do declarations if no definition exists
    if (!function_definitions[generate_name(this->type, this->name.value, this->args)].defined || this->statements.forward.size())
    {
        sprinta(write, "declare dso_local ", type_to_il_str[type], " @", this->name.value, "(");
        
        if (this->statements.forward.size() == 0)
        {
            for (auto arg : args)
            {
                sprinta(write, type_to_il_string[arg.type], ", ");
            }

            write->pop_back();
            write->pop_back();
            sprinta(write, ")");
        }
        else
        {
            for (auto arg : args)
            {
                sprinta(write, type_to_il_string[arg.type], "%", function_definitions[generate_name(this->type, this->name.value, this->args)].arg_to_il_name[arg.tok.value], ", ");
            }

            write->pop_back();
            write->pop_back();
            sprinta(write, ")");
        }
    }

    statements.visit(write);
}

void NoExpr::visit(std::string* write)
{
    return;
}

void UnaryOpNode::visit(std::string* write)
{
    switch (this->op)
    {
        case NodeKind::NOT:
        {
            forward->visit(write);
            if (result_type.t_kind == TypeKind::FLOAT) throw compiler_error("Invalid argument type ", to_string(result_type), " to unary expression: ", (int) this->op, "\n");
            sprinta(write, "%", next_temp, " = xor ", type_to_il_str[result_type], " ", result, ", -1\n");
            sprinta(&result, "%", next_temp++);
            break;
        }
        case NodeKind::NEG:
        {
            forward->visit(write);
            if (result_type.t_kind == TypeKind::FLOAT)
            {
                sprinta(write, "%", next_temp, " = fneg ", type_to_il_str[result_type], result, "\n");
                sprinta(&result, "%", next_temp++);
            }
            else
            {
                sprinta(write, "%", next_temp, " = sub ", type_to_il_str[result_type], " 0, ",  result, "\n");
                sprinta(&result, "%", next_temp++);
            }
            break;
        }
        case NodeKind::NOT:
        {
            forward->visit(write);

            const char* op = result_type == TypeKind::FLOAT ? "fcmp" : "icmp";
            const char* cmp = result_type == TypeKind::FLOAT ? "une" : "ne";
            sprinta(write, "%", next_temp, " = ", op, " ", cmp, " ", type_to_il_str[result_type], " ", result, ", ", zeroval[result_type.t_kind], "\n");
            next_temp++;
            sprinta(write, "%", next_temp, " = xor i1 %", next_temp - 1, ", true\n");
            next_temp++;
            sprinta(write, "%", next_temp, " = zext i1 %", next_temp - 1, " to i32\n");
            next_temp++;
            sprinta(&result, "%", next_temp);
            result_type = {TypeKind::INT, 4};
            break;
        }
        default: 
        {
            forward->visit(write);
            if (location.size() == 0) throw compiler_error("Error: Expected variable for operation: %d\n", (int) this->op);
            std::string op;
            if (this->op == NodeKind::PREFIXINC || this->op == NodeKind::POSTFIXINC) op = "add";
            else if (this->op == NodeKind::PREFIXDEC || this->op == NodeKind::POSTFIXDEC) op = "sub";
            else throw compiler_error("must have forgotten something");
            if (result_type.t_kind == TypeKind::FLOAT) op.insert(op.begin(), 'f');

            sprinta(write, "%", next_temp++, " = ", op, " ", type_to_il_str[result_type], " ", result, ", 1", after_decimal[result_type], "\n");
            if (this->op == NodeKind::PREFIXINC || this->op == NodeKind::PREFIXDEC) sprinta(&result, "%", next_temp - 1);
            if (this->op == NodeKind::POSTFIXINC || this->op == NodeKind::POSTFIXDEC) sprinta(&result, "%", next_temp - 2);
            location = "";
            break;
        }
    }
}

void BinaryOpNode::visit(std::string* write)
{
    std::unordered_map<NodeKind, std::string> op_to_str({
        {NodeKind::ADD, "add"},
        {NodeKind::SUB, "sub"},
        {NodeKind::MUL, "mul"},
        {NodeKind::DIV, "div"},
        {NodeKind::MOD, "rem"},
    });

    if (op_to_str.contains(op))
    {
        // Save both the type and the result location
        // If the first type doesnt equal the second type upscale (duh)
        // Use the updated rem values and do the op
    }


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
    // If the function is in global or stack scope
    if (var_map.size() == 0)
    {
        sprinta(write, "@", this->name.value, " = dso_local global ", type_to_il_str[this->type], " ");

        if (assign) assign->visit(write);
        else sprinta(write, "0", after_decimal[type.t_kind]);
        sprinta(write, ", align ", type.size_of, "\n\n");
    }  
    else 
    {
        if (var_map.back().contains(this->name.value)) throw compiler_error("Redefinition of local variable %s", this->name.value.c_str());
        var_map.back[this->name.value] = next_temp++;
        sprinta(write, "    %", var_map.back[this->name.value], " = alloca ", type_to_il_str[this->type], ", align ", this->type.size_of, "\n");
        if (assign) assign->visit(write);
        else default_value(this->type);
        if (result_type != this->type) cast(this->type, result_type, result);
        sprinta(write, "    store ", type_to_il_str[this->type], " ", result, ", ", type_to_il_str[this->type], "* %", var_map[this->name.value], ", align ", this->type.size_of, "\n");
    } 
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