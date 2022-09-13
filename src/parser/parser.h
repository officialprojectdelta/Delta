#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "node.h"
#include "lexer/token.h"

// The function that parses a set of tokens
Node* parse(Tokenizer& tokens);

