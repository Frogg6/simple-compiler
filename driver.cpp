/*
CSCI 490 Assign 1
By: Aaron Carreon (z1957830)
Due: 2/6/2025

Purpose: Test some basic controls from lexical anaylzer from data files
*/
#include "machine_code_gen.h"
#include "parser.h"
#include "lexical_analyzer.h"
#include "buffered_input.h"
#include "symbol_table.h"
#include "node.h"
#include "token.h"
#include "error.h"

#include <iostream>
#include <iomanip>
#include <string>

void print_text(std::string filename);
void print_token(Token);
void print_all_tokens(std::string filename);
void print_tree(Node *&p, int level);

/*
Purpose: To test the ncc so far developed.

Parameters/Returns: I mean its a main function.
*/
int main(int argc, char *argv[]) {
    std::string file;

    // Beginning argument intake
    if (argc > 1) {
        file = argv[1];
        if (argc > 2) {
            std::string mode = std::string(argv[2]);
            if (mode == "b")
                print_text(file);
            else if (mode == "l")
                print_all_tokens(file);
        }
    }
    else {
        std::cerr << "Not given a file to test\n";
        return 0;
    }

    Error e;
    e = parser_setup(file);
    if (e.id != ERR_OK) {
        throw_error(e);
    }

    Node *n;
    Token t;
    get_token(t);

    n = new Node();

    e = parse(n, t);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return 0;

    std::cout << "Code tree:" << std::endl;
    print_tree(n, 0);
    
    machine_setup();
    generate_instructions(n);
    execute_instructions();
    std::cout << "\n";

    parser_cleanup(n);
    machine_cleanup();
    lexical_cleanup();

    return 0;
}


// Prints out the source file given
void print_text(std::string filename) {
    buffer_setup(filename);

    char c;
    while (!buffer_eof()) {
        buffer_get_char_proceed(c);
        std::cout << c;
    }

    buffer_reset();
}


/*
Purpose: Prints out the current token t

Parameters: The token to print
*/
void print_token(Token t) {
    std::string id;
    switch(t.id) {
        case(NIL):
            id = "NULL";
            break;
        case(END_OF_FILE):
            id = "END_OF_FILE";
            break;
        case(PLUS):
            id = "PLUS";
            break; 
        case(MINUS):
            id = "MINUS";
            break; 
        case(MULT):
            id = "MULT";
            break;  
        case(DIV):
            id = "DIV";
            break;  
       case(IDENT):
            id = "INDENT";
            break;  
        case(EXP):
            id = "EXP";
            break;  
        case(LESS):
            id = "LESS";
            break;  
        case(LESS_EQ):
            id = "LESS_EQ";
            break;  
        case(GREATER):
            id = "GREATER";
            break;  
        case(GREATER_EQ):
            id = "GREATER_EQ";
            break;  
        case(EQUAL):
            id = "EQUAL";
            break;  
        case(NOT_EQUAL):
            id = "NOT_EQUAL";
            break;  
        case(ASSIGN):
            id = "ASSIGN";
            break;  
        case(NOT):
            id = "NOT";
            break;  
        case(LPAREN):
            id = "LPAREN";
            break;  
        case(RPAREN):
            id = "RPAREN";
            break;  
        case(LBRACE):
            id = "LBRACE";
            break;  
        case(RBRACE):
            id = "RBRACE";
            break;  
        case(LBRACKET):
            id = "LBRACKET";
            break;  
        case(RBRACKET):
            id = "RBRACKET";
            break;  
        case(AND):
            id = "AND";
            break;  
        case(OR):
            id = "OR";
            break;  
        case(DOT):
            id = "DOT";
            break;  
        case(AT):
            id = "AT";
            break;  
        case(INTEGER):
            id = "INTEGER";
            break;
        case(STRING):
            id = "STRING";
            break;
        case(COLON):
            id = "COLON";
            break;
        case(SEMICOLON):
            id = "SEMICOLON";
            break;
        case(COMMA):
            id = "COMMA";
            break;
        case(REAL):
            id = "REAL";
            break;
    }

    std::cout << std::left << std::setw(11) << id << " at (" << std::setfill('0') << std::right << std::setw(2) << t.line_number << "," << std::setw(2) << t.column_number << ")" << std::setfill(' ');

    // Special case values
    if (t.id == REAL) {
        std::cout << " with value: " << std::scientific << std::setprecision(2) << t.real_value;
    }
    else if (t.id == STRING || t.id == IDENT) {
        std::cout << " with value: " << t.value;
    }
    else if (t.id == INTEGER) {
        std::cout << " with value: " << t.int_value;
    }
    std::cout << std::endl;
}


void print_all_tokens(std::string filename) {
    // Create buffer
    lexical_setup(filename);

    Token t;
    Error e = get_token(t);

    // Gather tokens until EOF
    std::cout << "\n\nTOKEN STREAM\n\n";
    while (e.id != ERR_EOF) {
        if (e.id == ERR_OK) {
            print_token(t);
        }
        else {
            throw_error(e); 
        }
        e = get_token(t);
    }
    print_token(t);

    // Clean resources and exit
    lexical_cleanup(); 
}

void print_tree(Node *&p, int level) {
    if (p == nullptr)
        return;
    
    // Space the tiers of children
    for (int i = 0; i < level; i++) {
        std::cout << " ";
    }

    if (p->id == ADD)
        std::cout << "+ (add)\n";
    else if (p->id == SUBTRACT)
        std::cout << "- (sub)\n"; 
    else if (p->id == MULTIPLY)
        std::cout << "* (mult)\n"; 
    else if (p->id == DIVIDE)
        std::cout << "/ (div)\n";
    else if (p->id == NEG)
        std::cout << "- (neg)\n";
    else if (p->id == POS)
        std::cout << "+ (pos)\n";
    else if (p->id == MOD)
        std::cout << "% (mod)\n";
    else if (p->id == POWER)
        std::cout << "^ (pow)\n";
    else if (p->id == VARIABLE)
        std::cout << "variable: " << p->string_data << std::endl;
    else if (p->id == STR_ID)
        std::cout << p->string_data << std::endl;
    else if (p->id == VALUE)
        std::cout << p->int_data << std::endl;
    else if (p->id == STMT_BLOCK)
        std::cout << "statement block\n";
    else if (p->id == PRINT)
        std::cout << "print\n";
    else if (p->id == READ)
        std::cout << "read\n";
    else if (p->id == ASSIGN_NODE)
        std::cout << ":=\n"; // TODO
    else if (p->id == DECLARE_NODE)
        std::cout << "variable: " << p->string_data << std::endl;
    else if (p->id == BOOL)
        std::cout << p->string_data << std::endl;
    else if (p->id == LESS_NODE || p->id == LESS_EQ_NODE || p->id == GREATER_NODE || p->id == GREATER_EQ_NODE || p->id == EQ_NODE || p->id == NOT_EQ_NODE || p->id == OR_NODE || p->id == AND_NODE || p->id == NOT_NODE)
        std::cout << p->string_data << std::endl;
    else if (p->id == IF_NODE) 
        std::cout << "If" << std::endl;
    else if (p->id == WHILE_NODE)
        std::cout << "while" << std::endl;
    else if (p->id == NO_ID)
        std::cout << "What the";
    else
        std::cout << "Error: Node ID not found" << std::endl;

    if (!p->children.empty()) {
        for (long unsigned int i = 0; i < p->children.size(); i++) {
            print_tree(p->children[i], level + 2);
        }
    }
    print_tree(p->left, level + 2);
    print_tree(p->right, level + 2);

    /*
    if (p->string_data != "")
        std::cout << p->string_data << " ";
    else 
        std::cout << p->int_data << " ";
    */
}
