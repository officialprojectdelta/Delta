#include "util.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

// Remove a character (arg 1) from the string (arg 2)
// However, it will not remove characters surrounded by '
std::string remove_char(char character, const std::string& data)
{
    std::string newBuf;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] != character) 
        {
            if (data[i] == '\'')
            {
                newBuf.push_back(data[i]);
                i++;
                for (; data[i] != '\''; i++)
                {
                    newBuf.push_back(data[i]);
                }
                newBuf.push_back(data[i]);
            } 
            else
            {
                newBuf.push_back(data[i]);
            }
        }
    }

    return newBuf;
}

// Read data from a file loaded from filepath into a string
std::string read_file(const std::string& filepath)
{
    std::ifstream file(filepath);
    std::string str;

    if (!file.is_open()) throw std::runtime_error("Cannot find file");

    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    file.close();

    return str;
}

void write_file(const std::string& filepath, const std::string& data)
{
    std::ofstream file(filepath, std::ios::trunc);

    if (!file.is_open()) throw std::runtime_error("Somehow this file isn't opening");

    file.write(data.c_str(), data.size());

    file.close();
}