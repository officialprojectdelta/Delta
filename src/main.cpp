#include <iostream>
#include <chrono>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "symt/symt.h"
#include "codegen/codegen.h"
// #include "error/error.h"
#include "util.h"

// Compile the file
// Will take more params when done
int main(int argc, char** argv)
{
    try
    {
        auto startTm = std::chrono::high_resolution_clock::now();
        auto tokens = scan(read_file(argv[1]));
        std::cout << "tokens scanned" << std::endl;
        Node* node = parse_program(tokens);
        std::cout << "program parsed" << std::endl;
        generate_symtables(node);
        std::cout << "symtables generated" << std::endl;
        write_file(argv[2], codegen(node));
        std::cout << "Elapsed Time: " << (std::chrono::high_resolution_clock::now() - startTm).count() << "ns" << std::endl;
    }
    catch (compiler_error& e)
    {
        std::cout << e.what() << std::endl;
    } 
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}