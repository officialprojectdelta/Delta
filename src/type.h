#pragma once

// All of the data for typing in the compiler

// The different kinds of types (such as ints, floats, and bools)
enum class TypeKind
{
    INT
}; 

// The actual type entry in the node or symtable entry 
struct Type
{
    TypeKind typeKind;
    size_t sizeofNode;
    bool issigned; 
    size_t ptrCount;

    bool operator==(const Type& type) const { return this->typeKind == type.typeKind && this->sizeofNode == type.sizeofNode && this->issigned == type.issigned && this->ptrCount == type.ptrCount; }
    bool operator!=(const Type& type) const { return !(this->typeKind == type.typeKind && this->sizeofNode == type.sizeofNode && this->issigned == type.issigned && this->ptrCount == type.ptrCount); }
};