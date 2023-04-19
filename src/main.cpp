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
        Node* node = parse_program(tokens);
        generate_symtables(node);
        write_file(argv[2], codegen(node));
        std::cout << "Elapsed Time: " << (double) (std::chrono::high_resolution_clock::now() - startTm).count() / (double) 1000000 << "ms" << std::endl;
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