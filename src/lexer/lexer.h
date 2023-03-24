#pragma once

#include <unordered_map>
#include <vector> 

#include "token.h"

// Lex a string following the rules of the map
Tokenizer scan(std::string data);