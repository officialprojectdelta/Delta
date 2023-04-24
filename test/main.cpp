#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

std::string read_file(const std::string& filepath)
{
    std::ifstream file(filepath);
    std::string str;

    if (!file.is_open()) throw std::runtime_error("Cannot find file " + filepath);

    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    file.close();

    return str;
}

int main(void)
{
    system("make");
    system("clang -o test/tests/main.o -c test/tests/main.c -O2");

    for (auto file : std::filesystem::directory_iterator("test/tests/test"))
    {
        std::stringstream sstr;
        sstr << "clang -o test/tests/main test/tests/main.o " << file.path().string();
        system(sstr.str().c_str());
        sstr.str("");
        system("./test/tests/main 2>&1 | tee test/tests/clang-main.txt");
        system("rm -rf test/tests/main");
        sstr << "./bin/dcc " << file.path().string() << " " << file.path().stem().string() << ".ll";
        system(sstr.str().c_str());
        sstr.str("");
        sstr << "llc -o " << file.path().stem().string() << ".S " << file.path().stem().string() << ".ll";
        system(sstr.str().c_str());
        sstr.str("");
        sstr << "clang -o test/tests/main test/tests/main.o " << file.path().stem().string() << ".S";
        system(sstr.str().c_str());
        sstr.str("");
        system("./test/tests/main 2>&1 | tee test/tests/delta-main.txt");
        system("rm -rf test/tests/main");
        
        if (read_file("test/tests/delta-main.txt") != read_file("test/tests/clang-main.txt")) std::cout << "Test doesn't complete " << file.path().string();
        else std::cout << "Test does complete " << file.path().string();

        system("rm -rf test/tests/delta-main.txt test/test/clang-main.txt");
    }
}