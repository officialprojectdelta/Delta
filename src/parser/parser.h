#pragma once

#include "node/node.h"
#include "lexer/token.h"

// The function that parses a set of tokens
ProgramNode* parse_program(Tokenizer& tokens);

