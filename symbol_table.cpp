#include "symbol_table.h"
#include "storage.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;

Symbol *symbol_table[TABLE_SIZE];
int s_offset = 0; 

// Return memory address
int symbol_search(std::string givenName, Symbol_type givenType) {
    for (int i = 0; i < s_offset; i++) {
        if (symbol_table[i]->name == givenName && symbol_table[i]->symbolType == givenType) {
            // Symbol found
            return symbol_table[i]->storage_index;
        }
    }

    // Not found
    return -1;
}

// Return memory address
int symbol_insert(std::string newName, Symbol_type newType, Location_type newLocation) {
    Symbol* s = new Symbol;
    s->name = newName;
    s->symbolType = newType;
    s->locationType = newLocation;
    s->storage_index = s_offset;

    int* newInt = new int();
    store_int4(*newInt);

    symbol_table[s_offset++] = s;

    return s->storage_index;
}

void print_symbol_table() {
    cout << "\nSymbol table\n";
    for (int i = 0; i < s_offset; i++) {
        cout << "Symbol " << i << ": " << symbol_table[i]->name << " with int4: " << retrieve_int4(symbol_table[i]->storage_index) << endl;
    }
}
