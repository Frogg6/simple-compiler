/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: A token struct to store its ID and useful debugging location information
*/
#ifndef TOKEN_H
#define TOKEN_H
#include <string>

enum Token_ID {
    NIL,           // 0
    END_OF_FILE,
    PLUS,
    MINUS,
    MULT,
    DIV,           // 5
    IDENT,
    EXP,
    LESS,
    LESS_EQ,
    GREATER,       // 10
    GREATER_EQ,
    EQUAL,
    NOT_EQUAL,
    ASSIGN,
    NOT,           // 15
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,      // 20
    RBRACKET,
    AND,
    OR,
    DOT,
    AT,            // 25
    INTEGER,
    STRING,
    COLON,
    SEMICOLON,
    COMMA,         // 30
    REAL
};

struct Token {
    Token_ID id;
    int line_number, column_number;
    std::string value; // Used for select tokens
    long int_value;
    double real_value;
};

#endif /* TOKEN_H */
