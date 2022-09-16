#pragma once

#include <string>

#include "parser/node.h"
#include "symt/symt.h"

std::string& codegen(Node* node, Symtable& symtable);