/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: From a buffer, read in characters and transform them into tokens.
*/
#include "lexical_analyzer.h"
#include "buffered_input.h"
#include "token.h"
#include "error.h"


#include <iostream>

// More surface front function to call the buffered setup
Error lexical_setup(std::string filename) {
    return buffer_setup(filename);
}

// More surface front function to call the buffered cleanup
Error lexical_cleanup() {
    return buffer_cleanup();
}

/*
Purpose: Interpret the characters from a buffer into tokens.

Parameters: The next token which will store the value from function.
Returns: An error from invalid source code logic.
*/
Error get_token(Token & t) {
    char c;
    char c2 = ' ';
    char c3 = ' ';
    Error e = {.id = ERR_OK, .line_number = line_number, .column_number = column_number};

    // Reached end of file (if end of file is immediately after end of last token)
    if (is_eof_token(e, t))
        return e;

    // Main character to start checking against
    buffer_get_char(c);
    // ==============================
    // IGNORE EMPTY SPACE/COMMENTS
    // ==============================
    if (c == '<') {
        buffer_proceed();
        if (is_eof_token(e, t))
            return e;
        buffer_get_char(c2); // <
        
        buffer_proceed();
        if (is_eof_token(e, t))
            return e;
        buffer_get_char(c3); // -

        buffer_preceed();
        buffer_preceed();
    }
    while (isspace(c) || c == '#' || (c == '<' && c2 == '<' && c3 == '-')) {
        // Ignore whitespace
        while (isspace(c)) {
            buffer_proceed();
            if (is_eof_token(e, t))
                return e;
            buffer_get_char(c);
        }

        // Comment
        // Ignore until newline or error because there is none before eof
        if (c == '#') {
            buffer_proceed();
            // Premature eof, return error
            if (is_eof_token(e, t)) {
                e.id = INVALID_COMMENT; // Soft EOF
                return e;
            }

            // Loop until newline char has been found
            buffer_get_char(c);
            while (c != 10) {
                buffer_proceed();
                // Premature eof, return error
                if (is_eof_token(e, t)) {
                    e.id = INVALID_COMMENT; // Soft EOF
                    return e;
                }
                buffer_get_char(c);
            }
            // Start on char after the newline char
            buffer_proceed();
            if (is_eof_token(e, t))
                return e;
            buffer_get_char(c);
        }

        // Multiline comment
        // If first char is <, then check for the rest of them
        if (c == '<') {
            e = buffer_proceed();
            if (is_eof_token(e, t))
                return e;
            buffer_get_char(c2); // Possible '<'

            e = buffer_proceed();
            if (is_eof_token(e, t))
                return e;
            buffer_get_char(c3); // Possible '-'

            if (c == '<' && c2 == '<' && c3 == '-') { // Start of comment
                while (c != '-' || c2 != '>' || c3 != '>') { // Go until end of comment or EOF
                    e = buffer_proceed();
                    if (is_eof_token(e, t)) {
                        e.id = INVALID_COMMENT; // Soft EOF
                        return e;
                    }
                    c = c2;
                    c2 = c3;
                    buffer_get_char(c3);
                }
                // Get char after comment has ended
                e = buffer_proceed();
                if (is_eof_token(e, t))
                    return e;
                buffer_get_char(c);
            }
            else {
                buffer_preceed();
                buffer_preceed();
            }
        }
    }

    // ============================
    // START OF TOKEN
    // ===========================
    //Store start token position immediately
    t.line_number = line_number;
    t.column_number = column_number;
    e.line_number = line_number;
    e.column_number = column_number;

    // Integer token
    if (isdigit(c)) {
        long first_part;

        t.id = INTEGER;
        t.value.clear();

        // Loop until reaching a nondigit
        while (isdigit(c)) {
            t.value += c;

            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
            buffer_get_char(c);
        }
        // Everything before a . or e/E
        first_part = std::stol(t.value);
        t.value = std::to_string(first_part);

        // . makes it a real number
        if (c == '.') {
            double second_part;
            t.id = REAL;
            t.value += ".";

            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
            buffer_get_char(c);

            while(isdigit(c)) {
                t.value += c;

                e = buffer_proceed();
                if (e.id == ERR_EOF) { // Soft EOF
                    e.id = ERR_OK;
                    return e;
                }
                buffer_get_char(c);
            }

            second_part = std::stod(t.value);
            t.value = std::to_string(second_part);
            t.value.erase(t.value.find_last_not_of('0') + 1, std::string::npos);
            t.value.erase(t.value.find_last_not_of('.') + 1, std::string::npos);
            t.real_value = std::stod(t.value);
        }
        // No dot allowed directly before the exponent
        buffer_preceed();
        char possible_dot;
        buffer_get_char(possible_dot);
        e = buffer_proceed();
        if (e.id == ERR_EOF) { // Soft EOF
            e.id = ERR_OK;
            return e;
        }
        if ((c == 'e' || c == 'E') && possible_dot != '.') {
            std::string third_part;
            t.id = REAL;
            t.value += c;

            e = buffer_proceed();
            if (is_eof_token(e, t)) // TODO should this return a soft EOF?
                return e;
            buffer_get_char(c);

            if (c == '+' || c == '-') {
                t.value += c;
                e = buffer_proceed();
                if (is_eof_token(e, t)) // TODO should this return a soft EOF?
                    return e;
                buffer_get_char(c);
            }
            if (isdigit(c)) {
                while (isdigit(c)) {
                    third_part += c;

                    e = buffer_proceed();
                    if (e.id == ERR_EOF) { // Soft EOF
                        e.id = ERR_OK;
                        return e;
                    }
                    buffer_get_char(c);
                }
                t.value += std::to_string(std::stod(third_part));
                t.value.erase(t.value.find_last_not_of('0') + 1, std::string::npos);
                t.value.erase(t.value.find_last_not_of('.') + 1, std::string::npos);
                t.real_value = std::stod(t.value);
            }
            else {
                t.value = "INVALID REAL";
                e = {.id = INVALID_REAL};
                return e;
            }
        }

        // TODO MESSY
        t.int_value = first_part;
        e = {.id = ERR_OK};
        return e;
    }
    
    // Identifier token
    if (isalpha(c) || c == '_') {
        t.id = IDENT;
        t.value.clear();

        while (isalpha(c) || isdigit(c) || c == '_') {
            t.value += c;

            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
            //if (is_eof_token(e, t))
            //    return e;
            buffer_get_char(c);
        }
        e = {.id = ERR_OK};
        return e;
    }

    switch(c) {
        case('+'):
            t.id = PLUS;
            break;
        case('-'):
            t.id = MINUS;
            break;
        case('*'):
            t.id = MULT;
            break;
        case('/'):
            t.id = DIV;
            break;
        case('^'):
            t.id = EXP;
            break;
        case('<'):
            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
    
            buffer_get_char(c);
            if (c == '=')
                t.id = LESS_EQ;
            else {
                t.id = LESS;
                buffer_preceed();
            }
            break;
        case('>'):
            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }

            buffer_get_char(c);
            if (c == '=')
                t.id = GREATER_EQ;
            else {
                t.id = GREATER;
                buffer_preceed();
            }
            break; 
        case('='):
            t.id = EQUAL;
            break;
        case('~'):
            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
            buffer_get_char(c);
            if (c == '=') 
                t.id = NOT_EQUAL;
            else { 
                e.id = UNKNOWN_TOKEN;
                e.err_string = "~";
                return e;
            }
            break;
        case(':'):
            e = buffer_proceed();
            if (e.id == ERR_EOF) { // Soft EOF
                e.id = ERR_OK;
                return e;
            }
            buffer_get_char(c);
            if (c == '=') 
                t.id = ASSIGN;
            else {
                t.id = COLON;
                buffer_preceed();
            }
            break;
        case('!'):
            t.id = NOT;
            break;
        case('('):
            t.id = LPAREN;
            break;
        case(')'):
            t.id = RPAREN;
            break;
        case('{'):
            t.id = LBRACE;
            break;
        case('}'):
            t.id = RBRACE;
            break;
        case('['):
            t.id = LBRACKET;
            break;
        case(']'):
            t.id = RBRACKET;
            break;
        case('&'):
            t.id = AND;
            break;
        case('|'):
            t.id = OR;
            break;
        case('.'): // Has potential to be a real number
            e = buffer_proceed();
            if (is_eof_token(e, t))
                return e;
            buffer_get_char(c);

            if (isdigit(c)) {
                double second_part;
                t.id = REAL;
                t.value = ".";

                while (isdigit(c)) {
                    t.value += c;
                    e = buffer_proceed();
                    if (is_eof_token(e, t))
                        return e;
                    buffer_get_char(c);
                }
                second_part = std::stod(t.value);
                t.value = std::to_string(second_part);
                t.value.erase(t.value.find_last_not_of('0') + 1, std::string::npos);
                t.value.erase(t.value.find_last_not_of('.') + 1, std::string::npos);  
                t.real_value = std::stod(t.value);

                // No dot allowed directly before the exponent
                buffer_preceed();
                char possible_dot;
                buffer_get_char(possible_dot);
                e = buffer_proceed();
                if (is_eof_token(e, t))
                    return e;
                if ((c == 'e' || c == 'E') && possible_dot != '.') {
                    std::string third_part;
                    t.value += c;

                    e = buffer_proceed();
                    if (is_eof_token(e, t))
                        return e;
                    buffer_get_char(c);

                    if (c == '+' || c == '-') {
                        t.value += c;
                        e = buffer_proceed();
                        if (is_eof_token(e, t))
                            return e;
                        buffer_get_char(c);
                    }
                    if (isdigit(c)) {
                        while (isdigit(c)) {
                            third_part += c;

                            e = buffer_proceed();
                            if (is_eof_token(e, t))
                                return e;
                            buffer_get_char(c);
                        }
                        t.value += std::to_string(std::stod(third_part));
                        t.value.erase(t.value.find_last_not_of('0') + 1, std::string::npos);
                        t.value.erase(t.value.find_last_not_of('.') + 1, std::string::npos);
                        t.real_value = std::stod(t.value);
                    }
                    else {
                        t.value = "INVALID REAL";
                        e = {.id = INVALID_REAL};
                        return e;
                    } 
                }
            }
            else {
                t.id = DOT;
                buffer_preceed();
            }
            break;
        case('@'):
            t.id = AT;
            break;
        case(';'):
            t.id = SEMICOLON;
            break;
        case(','):
            t.id = COMMA;
            break;
        case('"'):
            t.id = STRING;
            t.value.clear();

            e = buffer_proceed();
            if (is_eof_token(e, t))
                return e;

            buffer_get_char(c);
            // Char is part of the string
            while (c != '"') {
                if (c == '\\') { // '\'
                    e = buffer_proceed();
                    if (is_eof_token(e, t))
                        return e;
                    buffer_get_char(c);
                    switch(c) {
                        case('n'):
                            t.value += 10;
                            break;
                        case('t'):
                            t.value += 9;
                            break;
                        case('r'):
                            t.value += 13;
                            break;
                        case('"'):
                            t.value += 34;
                            break;
                        case('\\'):
                            t.value += 92;
                            break;
                        case('a'):
                            t.value += '\a';
                            break;
                        case('b'):
                            t.value += 8;
                            break;
                        case(10):
                            break;
                        case('u'): {
                            std::string valueString;
                            for (int i = 0; i < 6; i++) {
                                e = buffer_proceed();
                                if (is_eof_token(e, t))
                                    return e;
                                buffer_get_char(c);

                                if (isdigit(c) || (c > 64 && c < 71) || (c > 96 && c < 103)) {
                                    valueString += c;
                                }
                                else {
                                    e.id = UNKNOWN_ESCAPE;
                                    return e;
                                }
                            }
                            int value = std::stoi(valueString, 0, 16);
       
                            if (value >= 0x0 && value <= 0x7f) {
                                t.value = (value & 0x7f);
                            }
                            else if (value >= 0x80 && value <= 0x7ff) {
                                // Add leftmost byte
                                t.value += (value >> 6) | 0xc0;
                                // Add the right byte
                                t.value += ((value & 0x3f) | 0x80);
                            }
                            else if (value >= 0x800 && value <= 0xffff) {
                                t.value += ((value >> 12) | 0xe0);
                                t.value += (((value >> 6) & 0x3f) | 0x80);
                                t.value += ((value & 0x3f) | 0x80);
                            }
                            else if (value >= 0x10000 && value <= 0x10ffff) {
                                std::cout << "LARGE UNICODE";
                                t.value += (value >> 18) | 0xf0;
                                t.value += ((value >> 12) & 0x3f) | 0x80;
                                t.value += ((value >> 6) & 0x3f) | 0x80;
                                t.value += ((value & 0x3f) | 0x80);
                            }
                            break;
                        }
                        default:
                            e.id = UNKNOWN_ESCAPE;
                            e.err_string = c;
                            throw_error(e);
                    }
                }
                else {
                    t.value += c;
                }
                e = buffer_proceed();
                if (is_eof_token(e, t)) 
                    return e;
                buffer_get_char(c);
            }
            break; 
        default:
            t.value = "Not handled"; // FOR TESTING
    }

    e = buffer_proceed();
    e.id = ERR_OK; // Soft EOF

    return e;
}

/*
Purpose: To determine if stream has reached end of file, and set error and token
         accordingly.

Parameters: An error to potentially set to EOF, and potential EOF token to set
Returns: Boolean, if EOF has been reached.
*/
bool is_eof_token(Error & e, Token & t) {
    if (buffer_eof()) {
        e = {.id = ERR_EOF, .line_number = line_number, .column_number = column_number};
        t = {.id = END_OF_FILE, .line_number = line_number, .column_number = column_number};
        t.value = "EOF TOKEN -- NORMAL"; // FOR TESTING 
        return true;
    }
    return false;
}
