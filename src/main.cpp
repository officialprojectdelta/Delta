#include <iostream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "symt/symt.h"
#include "codegen/codegen.h"
#include "error/error.h"
#include "util.h"

// Compile the file
// Will take more params when done
int main(int argc, char** argv)
{
    try
    {
        auto tokens = scan(readFile(argv[1]));

        Node* node = parse(tokens);

        Symtable table = genEntries(node);        
        
        writeFile(argv[2], codegen(node, table));
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