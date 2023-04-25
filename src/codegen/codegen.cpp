#include "codegen.h"

#include "util.h"
#include "symt/symt.h"

// std
#include <cstdio> 

// The output string
std::string output;

// The next unused temp value, increment after use
size_t next_temp;

// The result string, where something would go (so we dont need return values)
std::string result;

// The type of the result 
Type result_type;

// An additional result string for variable to put their actual location
std::string location;

// An additional value string for literals before they are converted into llvm form
std::string literal_value;

// The return type of the function (for return statements)
Type return_type;

// The stack declared variables and labels, (the vector is for multiple stack frames)
std::vector<std::unordered_map<std::string, std::pair<std::string, Type>>> var_map;

std::unordered_map<TypeKind, std::string> after_decimal({
    {TypeKind::FLOAT, ".000000e+00"},
    {TypeKind::INT, ""},
});

Type literal_cast(Type dst, Type src, const std::string& literal)
{
    if (literal[0] != '%' && src != dst)
    {
        if (dst.t_kind == TypeKind::INT || (dst.t_kind == TypeKind::UNSIGNED && dst.t_kind == TypeKind::UNSIGNED))
        {
            if (src.t_kind == TypeKind::FLOAT)
            {
                if (dst.size_of == 1) result = std::to_string((char) std::stod(literal_value));
                else if (dst.size_of == 2) result = std::to_string((short) std::stod(literal_value));
                else if (dst.size_of == 4) result = std::to_string((int) std::stod(literal_value));
                else result = std::to_string(std::stol(literal_value));

                src = dst;
            }
            else if (src.t_kind == TypeKind::INT)
            {
                if (dst.size_of > src.size_of)
                {
                    result = std::to_string((long) std::stol(literal_value));
                    src = dst;
                }
                else 
                {
                    if (dst.size_of == 1) result = std::to_string((char) std::stol(literal_value));
                    else if (dst.size_of == 2) result = std::to_string((short) std::stol(literal_value));
                    else if (dst.size_of == 4) result = std::to_string((int) std::stol(literal_value));
                    else result = literal_value;

                    src = dst;
                }
            }
            else if (src.t_kind == TypeKind::UNSIGNED)
            {
                if (dst.size_of > src.size_of)
                {
                    result = std::to_string(std::stol(literal_value));
                    src = dst;
                }
                else 
                {
                    if (dst.size_of == 1) result = std::to_string((unsigned char) std::stoul(literal_value));
                    else if (dst.size_of == 2) result = std::to_string((unsigned short) std::stoul(literal_value));
                    else if (dst.size_of == 4) result = std::to_string((unsigned int) std::stoul(literal_value));
                    else result = std::to_string((unsigned long) std::stoul(literal_value)); 

                    src = dst;
                }
            }
        }
        else if (dst.t_kind == TypeKind::UNSIGNED && src.t_kind == TypeKind::INT) 
        {
            if (dst.size_of > src.size_of)
            {
                result = std::to_string((long) std::stol(literal_value));
                src = dst;
            }
            else 
            {
                if (dst.size_of == 1) result = std::to_string((unsigned char) std::stoul(literal_value));
                else if (dst.size_of == 2) result = std::to_string((unsigned short) std::stoul(literal_value));
                else if (dst.size_of == 4) result = std::to_string((unsigned int) std::stoul(literal_value));
                else result = std::to_string((unsigned long) std::stoul(literal_value));

                src = dst;
            }
        }
        else if (dst.t_kind == TypeKind::UNSIGNED && src.t_kind == TypeKind::FLOAT)
        {
            if (dst.size_of == 1) result = std::to_string((unsigned char) std::stod(literal_value));
            else if (dst.size_of == 2) result = std::to_string((unsigned short) std::stod(literal_value));
            else if (dst.size_of == 4) result = std::to_string((unsigned int) std::stod(literal_value));
            else result = std::to_string((unsigned long) std::stod(literal_value));

            src = dst;
        }
        else if (dst.t_kind == TypeKind::FLOAT)
        {
            // For now no if statement is needed
            result = strfloat_to_hexfloat(literal_value, dst);
            src = dst;
        }
    }
    else 
    {
        return {TypeKind::NULLTP, 0};
    }

    return src;
}

void cast(std::string* write, Type dst, Type src, const std::string& temp_to_cast)
{
    result_type = literal_cast(dst, src, temp_to_cast);
    if (result_type != Type{TypeKind::NULLTP, 0}) 
    {
        location = ""; 
        literal_value = "";
        return;
    }

    std::string cast;

    if (src.t_kind == TypeKind::FLOAT && dst.t_kind == TypeKind::FLOAT)
    {
        if (src.size_of > dst.size_of) cast = "fptrunc";
        else cast = "fpext";
    }
    else if (src.t_kind == TypeKind::FLOAT && dst.t_kind == TypeKind::INT) cast = "fptosi";
    else if (dst.t_kind == TypeKind::FLOAT && src.t_kind == TypeKind::INT) cast = "sitofp";
    else if (src.t_kind == TypeKind::FLOAT && dst.t_kind == TypeKind::UNSIGNED) cast = "fptoui";
    else if (dst.t_kind == TypeKind::FLOAT && src.t_kind == TypeKind::UNSIGNED) cast = "uitofp";
    else 
    {
        if (src.size_of > dst.size_of) cast = "trunc";
        else if (src.size_of == dst.size_of) 
        {
            result_type = dst;
            location = ""; 
            literal_value = "";
            return;
        }
        else if (src.t_kind == TypeKind::INT) cast = "sext";
        else if (src.t_kind == TypeKind::UNSIGNED || dst.t_kind == TypeKind::UNSIGNED) cast = "zext";
        else 
        {
            result_type = dst;
            location = ""; 
            literal_value = "";
            return;
        }
    }

    sprinta(write, "    %", next_temp++, " = ", cast, " ", type_to_il_str[src], " ", temp_to_cast, " to ", type_to_il_str[dst], "\n");
    result = "%" + std::to_string(next_temp - 1);
    result_type = dst;
    location = ""; 
    literal_value = "";
}

std::string return_str(Type type)
{
    return "    ret " + type_to_il_str[type] + " 0" + after_decimal[type.t_kind] + "\n";
}

std::string codegen(Node* node)
{
    var_map.emplace_back();
    node->visit(&output);
    var_map.pop_back();

    // Fix double branches
    size_t i = output.find("    br");
    while (i != std::string::npos)
    {
        size_t j = output.find("    br", i + 12);
        size_t k = output.find(":", i);

        if (j == std::string::npos) break;
        if (k > j) output.erase(j, output.find("\n", j) - j + 1);
        i = j;
    }

    

    return output;
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
    var_map.emplace_back();

    for (auto x = forward.begin(); x != forward.end(); x++)
    {
        (*x)->visit(write);
    }

    var_map.pop_back();
}

void FunctionNode::visit(std::string* write)
{
    std::string init_variable_allocs;
    var_map.emplace_back();
    // Only do declarations if no definition exists
    if (!function_definitions[this->name.value].defined || this->statements.forward.size())
    {
        sprinta(write, "define dso_local ", type_to_il_str[type], " @", this->name.value, "(");
        
        if (this->statements.forward.size() == 0)
        {
            for (auto arg : args)
            {
                sprinta(write, type_to_il_str[arg.type], ", ");
            }

            if (args.size() != 0)
            {
                write->pop_back();
                write->pop_back();
            }
            
            sprinta(write, ") ");
        }
        else
        {
            next_temp = 0;
            for (auto arg : args)
            {
                sprinta(write, type_to_il_str[arg.type], " %", next_temp++, ", ");
            }
            next_temp++;
            
            // A proxy next_temp for args
            size_t arg_ctr = 0;
            for (auto arg : args)
            {
                var_map.back()[arg.tok.value] = {"%" + std::to_string(next_temp), arg.type};
                sprinta(&init_variable_allocs, "    %", next_temp, " = alloca ", type_to_il_str[arg.type], ", align ", arg.type.size_of, "\n");
                sprinta(&init_variable_allocs, "    store ", type_to_il_str[arg.type], " %", arg_ctr++, ", ", type_to_il_str[arg.type], "* %", next_temp++, ", align ", arg.type.size_of, "\n");
            }

            if (args.size() != 0)
            {
                write->pop_back();
                write->pop_back();
            }

            sprinta(write, ") ");
        }
    }

    if (statements.forward.size() != 0) 
    {   
        return_type = type;
        sprinta(write, "{\n", init_variable_allocs);
        statements.visit(write);
        sprinta(write, return_str(type));
        sprinta(write, "}\n\n");
    }

    var_map.pop_back();
}

void NoExpr::visit(std::string* write)
{
    result = "";
    result_type = Type{TypeKind::NULLTP, 0};
    return;
}

void UnaryOpNode::visit(std::string* write)
{
    switch (this->op)
    {
        case NodeKind::BITCOMPL:
        {
            forward->visit(write);
            if (result_type.t_kind == TypeKind::FLOAT) throw compiler_error("Invalid argument type ", (int) result_type.t_kind, " to unary expression: ", (int) this->op, "\n");
            sprinta(write, "    %", next_temp, " = xor ", type_to_il_str[result_type], " ", result, ", -1\n");
            sprinta(&result, "%", next_temp++);
            break;
        }
        case NodeKind::NEG:
        {
            forward->visit(write);
            if (result_type.t_kind == TypeKind::FLOAT)
            {
                sprinta(write, "    %", next_temp, " = fneg ", type_to_il_str[result_type], result, "\n");
                result = "%" + std::to_string(next_temp++);
            }
            else
            {
                sprinta(write, "    %", next_temp, " = sub ", type_to_il_str[result_type], " 0, ",  result, "\n");
                result = "%" + std::to_string(next_temp++);
            }
            break;
        }
        case NodeKind::NOT:
        {
            forward->visit(write);

            const char* op = result_type.t_kind == TypeKind::FLOAT ? "fcmp" : "icmp";
            const char* cmp = result_type.t_kind == TypeKind::FLOAT ? "une" : "ne";
            sprinta(write, "    %", next_temp, " = ", op, " ", cmp, " ", type_to_il_str[result_type], " ", result, ", 0", after_decimal[result_type.t_kind], "\n");
            next_temp++;
            sprinta(write, "    %", next_temp, " = xor i1 %", next_temp - 1, ", true\n");
            next_temp++;
            sprinta(write, "    %", next_temp, " = zext i1 %", next_temp - 1, " to i32\n");
            result = "%" + std::to_string(next_temp++);
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

            sprinta(write, "    %", next_temp++, " = ", op, " ", type_to_il_str[result_type], " ", result, ", 1", after_decimal[result_type.t_kind], "\n");
            sprinta(write, "    store ", type_to_il_str[result_type], " %", next_temp - 1, ", ", type_to_il_str[result_type], "* ", location, ", align ", result_type.size_of, "\n");
            if (this->op == NodeKind::PREFIXINC || this->op == NodeKind::PREFIXDEC) result = "%" + std::to_string(next_temp - 1);
            if (this->op == NodeKind::POSTFIXINC || this->op == NodeKind::POSTFIXDEC) result = "%" + std::to_string(next_temp - 2);
            break;
        }
    }

    location = ""; 
    literal_value = "";
}

void BinaryOpNode::visit(std::string* write)
{
    std::unordered_map<NodeKind, std::string> arith_op_to_str({
        {NodeKind::ADD, "add"},
        {NodeKind::SUB, "sub"},
        {NodeKind::MUL, "mul"},
        {NodeKind::DIV, "div"},
        {NodeKind::MOD, "rem"},
    });

    std::unordered_map<NodeKind, std::string> cmp_op_to_str({
        {NodeKind::EQ, "eq"},
        {NodeKind::NOTEQ, "ne"},
        {NodeKind::GREATER, "gt"},
        {NodeKind::GREATEREQ, "ge"},
        {NodeKind::LESS, "lt"},
        {NodeKind::LESSEQ, "le"},
    });

    // Convert types
    lhs->visit(write);
    std::string lhs_result = result;
    Type lhs_type = result_type;
    std::string lhs_location = location;
    std::string lhs_lit_val = literal_value;
    location = ""; 
    literal_value = "";

    rhs->visit(write);
    std::string rhs_result = result;
    Type rhs_type = result_type;
    std::string rhs_lit_val = literal_value;
    location = ""; 
    literal_value = "";

    if (arith_op_to_str.contains(op) || cmp_op_to_str.contains(op))
    {
        Type convert_to = expl_cast(lhs_type, rhs_type);

        if (lhs_type != convert_to)
        {
            literal_value = lhs_lit_val;
            cast(write, convert_to, lhs_type, lhs_result);
            lhs_result = result;
        }

        if (rhs_type != convert_to)
        {
            literal_value = rhs_lit_val;
            cast(write, convert_to, rhs_type, rhs_result);
            rhs_result = result;
        }

        if (arith_op_to_str.contains(op))
        {
            std::string before_char = "";
            if (op == NodeKind::DIV && convert_to.t_kind == TypeKind::INT) before_char = "s";
            if (op == NodeKind::DIV && convert_to.t_kind == TypeKind::UNSIGNED) before_char = "u";
            if (convert_to.t_kind == TypeKind::FLOAT) before_char = "f";
            // Output operation
            sprinta(write, "    %", next_temp++, " = ", before_char, arith_op_to_str[op], " ", type_to_il_str[convert_to], " ", lhs_result, ", ", rhs_result, "\n");
            result = "%" + std::to_string(next_temp - 1);
        } 
        else
        {
            std::string before_char = "";
            std::string before_cmp_char = "";
            if (convert_to.t_kind == TypeKind::FLOAT)
            {
                before_char = "f";
                before_cmp_char = "o";
            }
            else if (convert_to.t_kind == TypeKind::INT)
            {
                before_char = "i";
                if (op != NodeKind::EQ && op != NodeKind::NOTEQ) before_cmp_char = "s";
            }
            else if (convert_to.t_kind == TypeKind::UNSIGNED)
            {
                before_char = "i";
                if (op != NodeKind::EQ && op != NodeKind::NOTEQ) before_cmp_char = "u";
            }

            sprinta(write, "    %", next_temp++, " = ", before_char, "cmp ", before_cmp_char, cmp_op_to_str[op], " ", type_to_il_str[convert_to], " ", lhs_result, ", ", rhs_result, "\n");
            result = "%" + std::to_string(next_temp - 1);
            result_type = {TypeKind::BOOL, 1};
        }
    }
    else
    {
        if (lhs_location.size() == 0) throw compiler_error("%s is not a variable", lhs_result.c_str());
        literal_value = rhs_lit_val;
        if (lhs_type != rhs_type) cast(write, lhs_type, rhs_type, rhs_result);
        sprinta(write, "    store ", type_to_il_str[result_type], " ", result, ", ", type_to_il_str[result_type], "* ", lhs_location, ", align ", result_type.size_of, "\n");
    }

    location = ""; 
    literal_value = "";
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
    literal_value = this->value.value;
    result = this->value.value;
    if (this->type.t_kind == TypeKind::FLOAT) result = strfloat_to_hexfloat(this->value.value, this->type);
    result_type = this->type;
    location = ""; 
}

void VarNode::visit(std::string* write)
{
    // Lazy but works, load and give location (when storing ofcourse only location is needed, but ir removes unnecessary load)
    for (auto i = var_map.rbegin(); i != var_map.rend(); i++)
    {
        if (i->contains(this->name.value))
        {
            sprinta(write, "    %", next_temp++, " = load ", type_to_il_str[(*i)[this->name.value].second], ", ", type_to_il_str[(*i)[this->name.value].second], "* ", (*i)[this->name.value].first, ", align ", (*i)[this->name.value].second.size_of, "\n");
            result = "%" + std::to_string(next_temp - 1);
            result_type = (*i)[this->name.value].second;
            location = (*i)[this->name.value].first;
            return;
        }
    }

    throw compiler_error("Variable %s not declared\n", this->name.value.c_str());
}

void FuncallNode::visit(std::string* write)
{
    // Check if function exists
    if (!function_definitions.contains(this->name.value)) throw compiler_error("Function %s not declared\n", this->name.value.c_str());

    // Check if arguments are correct
    if (this->args.size() != function_definitions[this->name.value].args.size()) throw compiler_error("Function %s called with wrong number of arguments\n", this->name.value.c_str());

    std::string funcall_args;
    
    // Check if arguments are correct and call the function with them, cast if needed
    size_t j = 0;
    for (auto i = args.begin(); i != args.end(); i++, j++)
    {
        (*i)->visit(write);
        if (result_type != function_definitions[this->name.value].args[0].type) cast(write, function_definitions[this->name.value].args[0].type, result_type, result);
        sprinta(&funcall_args, type_to_il_str[result_type], " ", result, ", ");
    }

    sprinta(write, "    %", next_temp++, " = call ", type_to_il_str[function_definitions[this->name.value].type], " @", this->name.value, "(", funcall_args);

    if (args.size() != 0) 
    {
        write->pop_back();
        write->pop_back();
    }
    sprinta(write, ")\n");

    result = "%" + std::to_string(next_temp - 1);
    result_type = function_definitions[this->name.value].type;
    location = ""; 
    literal_value = "";
}

void DeclNode::visit(std::string* write)
{
    // If the function is in global or if it is in stack scope
    if (var_map.size() == 1)
    {
        sprinta(write, "@", this->name.value, " = dso_local global ", type_to_il_str[this->type], " ");

        if (assign) 
        {
            assign->visit(write);
            // Only literal cast b/c no code can be executed
            result_type = literal_cast(this->type, result_type, result);
            if (result_type == Type{TypeKind::NULLTP, 1}) throw compiler_error("Global variable can only be declared as a literal");
            literal_value = "";
            location = "";
            sprinta(write, result);
        }
        else sprinta(write, "0", after_decimal[type.t_kind]);
        sprinta(write, ", align ", type.size_of, "\n\n");
        var_map.back()[this->name.value] = {std::string("@").append(this->name.value), this->type};
    }  
    else 
    {
        if (var_map.back().contains(this->name.value)) throw compiler_error("Redefinition of local variable %s", this->name.value.c_str());
        var_map.back()[this->name.value] = {"%" + std::to_string(next_temp++), this->type};
        sprinta(write, "    ", var_map.back()[this->name.value].first, " = alloca ", type_to_il_str[this->type], ", align ", this->type.size_of, "\n");
        if (assign) 
        {
            assign->visit(write);
            if (result_type != this->type) cast(write, this->type, result_type, result);
            sprinta(write, "    store ", type_to_il_str[this->type], " ", result, ", ", type_to_il_str[this->type], "* ", var_map.back()[this->name.value].first, ", align ", this->type.size_of, "\n");
        }
        else
        {
            sprinta(write, "    store ", type_to_il_str[this->type], " 0", after_decimal[this->type.t_kind], ", ", type_to_il_str[this->type], "* ", var_map.back()[this->name.value].first, ", align ", this->type.size_of, "\n");
        }
    } 
}

void BreakNode::visit(std::string* write)
{
    // Stupid hacks
    sprinta(write, "{break}\n");
}

void ContinueNode::visit(std::string* write)
{
    // Stupid hacks
    sprinta(write, "{continue}\n");
}

void RetNode::visit(std::string* write)
{
    value->visit(write);
    if (result_type != return_type) cast(write, return_type, result_type, result);
    sprinta(write, "    ret ", type_to_il_str[result_type], " ", result, "\n");
}

void IfNode::visit(std::string* write)
{
    condition->visit(write);
    location = ""; 
    literal_value = "";
    
    // Make sure that we are branching with a boolean value
    if (result_type != Type{TypeKind::BOOL, 1}) 
    {
        if (result_type.t_kind == TypeKind::INT) sprinta(write, "    %", next_temp++, " = icmp ne ", type_to_il_str[result_type], " ", result, ", 0\n");
        else if (result_type.t_kind == TypeKind::FLOAT) sprinta(write, "    %", next_temp++, " = fcmp une ", type_to_il_str[result_type], " ", result, ", 0.000000e+00\n");
        else throw compiler_error("Invalid type %s\n", type_to_il_str[result_type].c_str());

        result = "%" + std::to_string(next_temp - 1);
        result_type = Type{TypeKind::BOOL, 1};
    }
    
    // Do the first branch
    size_t label_save = next_temp;
    sprinta(write, "    br i1 ", result, ", label %", next_temp++, ", label %");
    
    // Save the code for the if true statement, so that next_temp gets incremented properly
    std::string if_true_execute;
    statement->visit(&if_true_execute);
    location = ""; 
    literal_value = "";

    // Save the label for the end of the if true statment (leads to else or end)
    sprinta(write, next_temp++, "\n\n", label_save, ":\n");
    label_save = next_temp - 1;
    
    // Save the code for the else statement, so that next_temp gets incremented properly (if there is one)
    std::string else_execute; 
    if (else_stmt) else_stmt->visit(&else_execute);

    // Find our end label (could be the old label_save or the next_temp)
    size_t end_label = else_stmt ? next_temp++ : label_save;

    // Print the code for the if true statement
    sprinta(write, if_true_execute);
    sprinta(write, "    br label %", end_label, "\n\n");
    sprinta(write, label_save, ":\n");
    
    // Print the code for the if else statement (if there is one)
    if (else_stmt)
    {
        sprinta(write, else_execute);
        sprinta(write, "    br label %", end_label, "\n\n");
        sprinta(write, end_label, ":\n");
    }

    location = ""; 
    literal_value = "";
}

void ForNode::visit(std::string* write)
{
    initial->visit(write);
    location = ""; 
    literal_value = "";

    var_map.emplace_back();

    size_t begin_loop = next_temp++;
    sprinta(write, "    br label %", begin_loop, "\n\n");
    sprinta(write, begin_loop, ":\n");

    condition->visit(write);
    location = ""; 
    literal_value = "";

    // Make sure that we are branching with a boolean value
    if (result_type != Type{TypeKind::BOOL, 1} && result_type != Type{TypeKind::NULLTP, 0}) 
    {
        if (result_type.t_kind == TypeKind::INT) sprinta(write, "    %", next_temp++, " = icmp ne ", type_to_il_str[result_type], " ", result, ", 0\n");
        else if (result_type.t_kind == TypeKind::FLOAT) sprinta(write, "    %", next_temp++, " = fcmp une ", type_to_il_str[result_type], " ", result, ", 0.000000e+00\n");
        else throw compiler_error("Invalid type %s\n", type_to_il_str[result_type].c_str());

        result = "%" + std::to_string(next_temp - 1);
        result_type = Type{TypeKind::BOOL, 1};
    }
    else if (result_type == Type{TypeKind::NULLTP, 0})
    {
        result = "1";
        result_type = Type{TypeKind::BOOL, 1};
    }

    std::string condition_loc = result;
    size_t check_condition = next_temp++;
    size_t end_loop_label;
    std::string execute;
    std::string end_loop;

    statement->visit(&execute);
    location = ""; 
    literal_value = "";

    end_loop_label = next_temp++;

    end->visit(&end_loop);
    location = ""; 
    literal_value = "";

    size_t i = execute.find("{break}");
    while (i != std::string::npos)
    {
        execute.erase(i, 7);
        std::string input_this;
        sprinta(&input_this, "    br label %", next_temp + 1, "\n\n");
        execute.insert(i, input_this);
        i = execute.find("{break}", i);
    }

    i = execute.find("{continue}");
    while (i != std::string::npos)
    {
        execute.erase(i, 10);
        std::string input_this;
        sprinta(&input_this, "    br label %", next_temp, "\n\n");
        execute.insert(i, input_this);
        i = execute.find("{continue}", i);
    }

    sprinta(write, "    br i1 ", condition_loc, ", label %", check_condition, ", label %", next_temp, "\n\n");
    sprinta(write, check_condition, ":\n");
    sprinta(write, execute);
    sprinta(write, "    br label %", end_loop_label, "\n\n");
    sprinta(write, end_loop_label, ":\n");
    sprinta(write, end_loop);
    sprinta(write, "    br label %", begin_loop, "\n\n");
    sprinta(write, next_temp++, ":\n");

    var_map.pop_back();
}

void WhileNode::visit(std::string* write)
{
    size_t begin_loop = next_temp++;
    sprinta(write, "    br label %", begin_loop, "\n\n");
    sprinta(write, begin_loop, ":\n");

    if (do_on)
    {
        statement->visit(write);
        location = ""; 
        literal_value = "";
        condition->visit(write);
        location = ""; 
        literal_value = "";

        // Make sure that we are branching with a boolean value
        if (result_type != Type{TypeKind::BOOL, 1}) 
        {
            if (result_type.t_kind == TypeKind::INT) sprinta(write, "    %", next_temp++, " = icmp ne ", type_to_il_str[result_type], " ", result, ", 0\n");
            else if (result_type.t_kind == TypeKind::FLOAT) sprinta(write, "    %", next_temp++, " = fcmp une ", type_to_il_str[result_type], " ", result, ", 0.000000e+00\n");
            else throw compiler_error("Invalid type %s\n", type_to_il_str[result_type].c_str());

            result = "%" + std::to_string(next_temp - 1);
            result_type = Type{TypeKind::BOOL, 1};
        }

        size_t i = write->find("{break}");
        while (i != std::string::npos)
        {
            write->erase(i, 7);
            std::string input_this;
            sprinta(write, "    br label %", next_temp);
            write->insert(i, input_this);
            i = write->find("{break}", i);
        }

        i = write->find("{continue}");
        while (i != std::string::npos)
        {
            write->erase(i, 10);
            std::string input_this;
            sprinta(&input_this, "    br label %", next_temp);
            write->insert(i, input_this);
            i = write->find("{continue}", i);
        }

        sprinta(write, "    br i1 ", result, ", label %", begin_loop, ", label %", next_temp, "\n\n");
        sprinta(write, next_temp++, ":\n");
    }
    else
    {
        condition->visit(write);
        location = ""; 
        literal_value = "";

        // Make sure that we are branching with a boolean value
        if (result_type != Type{TypeKind::BOOL, 1}) 
        {
            if (result_type.t_kind == TypeKind::INT) sprinta(write, "    %", next_temp++, " = icmp ne ", type_to_il_str[result_type], " ", result, ", 0\n");
            else if (result_type.t_kind == TypeKind::FLOAT) sprinta(write, "    %", next_temp++, " = fcmp une ", type_to_il_str[result_type], " ", result, ", 0.000000e+00\n");
            else throw compiler_error("Invalid type %s\n", type_to_il_str[result_type].c_str());

            result = "%" + std::to_string(next_temp - 1);
            result_type = Type{TypeKind::BOOL, 1};
        }

        std::string condition_loc = result;
        size_t check_condition = next_temp++;
        std::string execute;

        statement->visit(&execute);
        location = ""; 
        literal_value = "";

        size_t i = execute.find("{break}");
        while (i != std::string::npos)
        {
            execute.erase(i, 7);
            std::string input_this;
            sprinta(&input_this, "    br label %", next_temp);
            execute.insert(i, input_this);
            i = execute.find("{break}", i);
        }

        i = execute.find("{continue}");
        while (i != std::string::npos)
        {
            execute.erase(i, 10);
            std::string input_this;
            sprinta(&input_this, "    br label %", begin_loop);
            execute.insert(i, input_this);
            i = execute.find("{continue}", i);
        }

        sprinta(write, "    br i1 ", condition_loc, ", label %", check_condition, ", label %", next_temp, "\n\n");
        sprinta(write, check_condition, ":\n");
        sprinta(write, execute);
        sprinta(write, "    br label %", begin_loop, "\n\n");
        sprinta(write, next_temp++, ":\n");
    }
}