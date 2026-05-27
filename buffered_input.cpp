/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: Creates a buffer from a source file, implements a reading interface into it.
         Also features debug printing functions.
*/
#include "buffered_input.h"
#include "error.h"
#include <string>
#include <iostream>
#include <fstream>

// line_number and column_nmber tracks current placement
int line_number = 1, column_number = 1;
int buffer_index = 0;
int length, line_count;
char * buffer = NULL; // Set to null to check if changed later
int * line_starts = NULL, * line_lengths = NULL;


/*
Purpose: Takes the source file and puts it into a buffer array of characters.
         Also sets up position trackers for lines and lengths of lines.

Parameters: A filename to be opened.
Returns: An error if file can't be open, read, ect.
*/
Error buffer_setup(std::string filename) {
    std::ifstream in_file;
    Error e;

    // Open the file
    in_file.open(filename, std::ifstream::binary);
    // If file cannot be found, return error
    if (!in_file.good()) {
        e = {.id = FILE_OPEN, .err_string = filename};
        return e;
    }
    
    // Get length of file
    in_file.seekg(0, in_file.end);
    length = in_file.tellg();
    in_file.seekg(0, in_file.beg);

    // Create buffer equal to file length
    buffer = new char [length];

    // Read file into buffer
    in_file.read(buffer, length);
    // If file cannot be read, return error
    if (!in_file.good()) { 
        e = {.id = FILE_READ};
        return e;
    }
    // Close file, now we work with the buffer
    in_file.close();

    // Find number of lines + 1 for human-friendly accessibility
    line_count = 1;
    for (int i = 0; i < length; i++) {
        if (buffer[i] == 10)
            line_count++;
    }
    if (buffer[length - 1] != 10)
        line_count++;

    // Initialize arrays to number of lines + 1
    line_starts = new int [line_count + 1];
    line_lengths = new int [line_count + 1];

    line_count = 1;
    line_starts[1] = 0;
    int line_length = 0;
    // Loop through the whole file
    for (int i = 0; i < length; i++) {
        line_length++;
        // If new-line character found, store line data
        if (buffer[i] == 10) {
            line_starts[line_count + 1] = i + 1; // Set next line to start right after end line
            line_lengths[line_count] = line_length; // Set current line to length accrued
            line_length = 0; // Reset length
            line_count++;
        }
    } 
    // Edge case: last line doesn't have a new-line
    if (buffer[length - 1] != 10) {
        line_lengths[line_count] = line_length;
    }

    e = {.id = ERR_OK};
    return e;
}

/*
Purpose: Deletes all dynamically allocated memory created from buffer

Returns: Error if memory was never created in the first place
*/
Error buffer_cleanup() {
    Error e;
    if (buffer != NULL && line_starts != NULL && line_lengths != NULL) {
        delete[] buffer;
        delete[] line_starts;
        delete[] line_lengths;

        e = {.id = ERR_NULL};
        return e;
    }
    else {
        e = {.id = ERR_OK};
        return e;
    }
}

// Function that returns whether or not the end of file has been reached.
bool buffer_eof() {
    return buffer_index >= length;
}

/*
Purpose: Gets the current character at buffer position.

Parameters: The character read
Returns: An error if no character left to read.
*/
Error buffer_get_char(char & c) {
    Error e;

    if (buffer_eof()) {
        e = {.id = ERR_EOF};
    }
    else {
        c = buffer[buffer_index];
        e = {.id = ERR_OK};
    }
    return e;
}

/*
Purpose: Advances the buffer position by 1

Returns: An error if reached end of file
*/
Error buffer_proceed() {
    Error e = {.line_number = line_number, .column_number = column_number};

    // Advance index by 1
    buffer_index++;

    // Advance position trackers
    column_number++;
    if (column_number > line_lengths[line_number] && buffer[buffer_index - 1] == 10) {
        column_number = 1;
        line_number++;
    }

    // Check to see if we made it to the end-of-file
    if (buffer_eof()) {
        e = {.id = ERR_EOF, .line_number = line_number, .column_number = column_number};
    }
    else {
        e = {.id = ERR_OK};
    }
    return e;
}

/*
Purpose: To get the current buffer's character and then advance 1 read position

Parameters: The character read
Returns: An error if end of file has been reached
*/
Error buffer_get_char_proceed(char & c) {
    Error e = buffer_get_char(c);

    if (e.id != ERR_OK)
        return e;
    return buffer_proceed();
}

// Function to decrement readiing position by 1, returns error if before 1st position
Error buffer_preceed() {
    Error e;

    // Decrement index by 1
    buffer_index--;

    // Adjust position counters backwards
    column_number--;
    if (column_number < 1) {
        column_number = line_lengths[--line_number];
        // If before first position possible
        if (line_number < 1) {
            e = {.id = OUT_OF_BOUNDS};
            return e;
        }
    }
    
    e = {.id = ERR_OK};
    return e;
}

// Function to reset the reading position values
Error buffer_reset() {
    buffer_index = 0;
    line_number = 1;
    column_number = 1;

    return {ERR_OK};
}

/*
Purpose: Store a given line number of a source file

Parameters: The line number to print and result to store the output in
Returns: An error if line is not possible
*/
Error buffer_line_contents(int line_number, std::string & line) {
    Error e;

    if (line_number < 1 || line_number >= line_count) {
        e = {.id = OUT_OF_BOUNDS};
        return e;
    }
    
    line = "";
    for (int i = 0; i < line_lengths[line_number]; i++) {
        line += char(buffer[line_starts[line_number] + i]);
    }

    e = {.id = ERR_OK};
    return e;
}
