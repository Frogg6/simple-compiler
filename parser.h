/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Converts a parse tree of arithmetic expressions into machine code to
          compute values from.
*/
#ifndef PARSER_H
#define PARSER_H

#include "error.h"
#include "token.h"
#include "node.h"
#include <string>

Error parser_setup(std::string filename);
void parser_cleanup(Node *&p);

Error parse(Node *&p, Token &t);

bool is_eof();

Error stmt_level(Node *&p);

Error log_or_level(Node *&p);
Error log_and_level(Node *&p);
Error log_not_level(Node *&p);

Error relative_level(Node *&p);

Error addsub_level(Node *&p);
Error multdiv_level(Node *&p);
Error exp_level(Node *&p);
Error data_level(Node *&p);


#endif /* PARSER_H */
