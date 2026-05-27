/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: A error struct to capture and display errors to the user.
*/
#ifndef ERROR_H
#define ERROR_H

#include <string>

enum Error_ID { // All less than 0, possible integration with buf I/O and char ASCII
    ERR_OK = 0,
    ERR_EOF = -1,
    FILE_OPEN = -2,
    FILE_READ = -3,
    FILE_CLOSE = -4,
    OUT_OF_BOUNDS = -5,
    ERR_NULL = -6,
    INVALID_INT = -7,
    INVALID_REAL = -8,
    UNKNOWN_TOKEN = -9,
    INVALID_COMMENT = -10,
    UNKNOWN_ESCAPE = -11,
    EXPECTED_TOKEN = -12,
    DUPLICATE_DECLARE = -13
};

struct Error {
        Error_ID id;
        int line_number, column_number; // Might to useful to put it into a map

        // Optional for putting into error message
        char err_char;
        int err_int;
        std::string err_string;
};

void throw_error(Error e);  
void display_code_error(Error e);

#endif /* ERROR_H */
