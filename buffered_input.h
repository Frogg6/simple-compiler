/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: Creates a buffer from a source file, implements a reading interface into it.
         Also features debug printing functions.
*/
#ifndef BUFFERED_INPUTI_H
#define BUFFERED_INPUT_H

#include "error.h"
#include <string>

extern int line_number, column_number;

Error buffer_setup(std::string filename);
Error buffer_cleanup();
bool buffer_eof();

Error buffer_get_char(char &);
Error buffer_proceed();
Error buffer_get_char_proceed(char &);
Error buffer_preceed();
Error buffer_reset();

Error buffer_line_contents(int, std::string &);

#endif /* BUFFERED_INPUT_H */ 
