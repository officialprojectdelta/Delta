#pragma once

#include <string>
#include <sstream>
#include <iostream>

// Remove characters from a string
std::string remove_char(char character, const std::string& data);

// Read data from a file into a string
std::string read_file(const std::string& filepath);

// Write data to a file (existing or created) from a string
void write_file(const std::string& filepath, const std::string& data);

// sprint (prints to a string, using std::stringstream, the laziest thing in the world)
template <typename Arg>
void sprinta(std::string* write, Arg arg)
{
    std::stringstream str;
    str << arg;
    write->append(str.str());
    return;
}

template <typename Arg, typename... Args>
void sprinta(std::string* write, Arg arg, Args... args)
{
    std::stringstream str;
    str << arg;
    write->append(str.str());
    sprinta(write, args...);
    return;
}

// oprinta (prints to std output, using std::iostream, the laziest thing in the world)
template <typename Arg>
void oprinta(Arg arg)
{
    std::cout << arg;
}

template <typename Arg, typename... Args>
void oprinta(Arg arg, Args... args)
{
    std::cout << arg;
    oprinta(args...);
    return;
}