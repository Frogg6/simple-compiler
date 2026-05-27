#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>

const int TABLE_SIZE = 400;

enum Symbol_type {
    SYMBOL_NULL,
    SYMBOL_VAR
};

enum Location_type {
    SYMBOL_MEMORY,
    SYMBOL_REGISTER,
    SYMBOL_STACK
};

struct Symbol {
    std::string name;
    Symbol_type symbolType;
    Location_type locationType;
    
    int storage_index;
    // Register label?
    // int stack_offset;
};



int symbol_search(std::string givenName, Symbol_type givenType);
int symbol_insert(std::string newName, Symbol_type newType, Location_type newLocation);
void print_symbol_table();
    

#endif /* SYMBOL_TABLE_H */
