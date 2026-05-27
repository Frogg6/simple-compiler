/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: A error struct to capture and display errors to the user.
*/
#include "error.h"
#include "buffered_input.h"
#include <iostream>
#include <string>

using std::cerr;
using std::string;

/*
Purpose: Given error ID, display corresponding error to user

Parameters: An error to display
*/
void throw_error(Error e) {
    switch(e.id) {
        case(ERR_OK):
            break;
        case(FILE_OPEN):
            std::cerr << "Error: cannot open file \"" << e.err_string << "\"\n";
            break;
        case(FILE_READ):
            std::cerr << "Error: cannot read file\n";
            break;
        case(FILE_CLOSE):
            std::cerr << "Error: cannot close file\n";
            break;
        case(OUT_OF_BOUNDS):
            std::cerr << "Error: attempting to access location out of bounds\n";
            break;
        case(INVALID_INT):
            std::cerr << "Error: invalid integer number\n";
            break;
        case(INVALID_REAL):
            std::cerr << "Error: invalid real number\n";
            break;
        case(UNKNOWN_TOKEN):
            buffer_preceed();
            std::cerr << "Error: unknown token " << e.err_string << " at (" << line_number << ", " << column_number << ")\n";
            display_code_error(e);
            buffer_proceed();
            break;
        case(INVALID_COMMENT):
            std::cerr << "Error: premature comment termination\n";
            break;
        case(UNKNOWN_ESCAPE):
            std::cerr << "Error: unknown escape character " << e.err_string << "\n";
            break;
        case(EXPECTED_TOKEN):
            buffer_preceed();
            std::cerr << "Error: expected '" << e.err_string << "' at (" << line_number << ", " << column_number << ")\n";
            display_code_error(e);
            buffer_proceed();
            break;
        case(DUPLICATE_DECLARE):
            buffer_preceed();
            std::cerr << "Error: duplicate symbol '" << e.err_string << "' at (" << line_number << ", " << column_number << ")\n";
            display_code_error(e);
            buffer_proceed();
            break;
        default:
            std::cerr << "ERROR: This error is not handled yet: " << e.id << "\n";
            break;
    }
}

void display_code_error(Error e) {
    string s;
    buffer_line_contents(line_number, s);

    cerr << s;
    for (int i = 1; i < column_number; i++) 
        cerr << "-";
    cerr << "^\n";
}
