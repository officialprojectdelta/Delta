#pragma once

#include <string>

// Remove characters from a string
std::string removeChar(char character, const std::string& data);

// Read data from a file into a string
std::string readFile(const std::string& filepath);

// Write data to a file (existing or created) from a string
void writeFile(const std::string& filepath, const std::string& data);