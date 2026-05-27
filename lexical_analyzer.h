/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: From a buffer, read in characters and transform them into tokens.
*/
#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "token.h"
#include "error.h"
#include <string>

Error lexical_setup(std::string);
Error lexical_cleanup();

Error get_token(Token & t);

bool is_eof_token(Error & e, Token & t);

#endif /* LEXICAL_ANALYZER_H */
