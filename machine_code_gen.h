/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Converts a parse tree of arithmetic expressions into machine code to
          compute values from.
*/
#ifndef MACHINE_CODE_GEN_H
#define MACHINE_CODE_GEN_H

#include "node.h"

void machine_setup();
void machine_cleanup();

void move_symbol_eax(Node* n);

void generate_instructions(Node *&n);
void parse_tree(Node *&n);
void execute_instructions();

void print_int4(int);
void print_string(char *);
void print_bool(int);
int read_int4();

void add_int4(int);
void add_int8(long long int);

void get_tree_byte_size(Node *p, int &size);

#endif /* MACHINE_CODE_GEN_H */
