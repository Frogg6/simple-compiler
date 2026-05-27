/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Parses through tokens to create arithmetic expressions in binary tree format.
*/
#include "parser.h"
#include "lexical_analyzer.h"
#include "storage.h"
#include "token.h"
#include "error.h"

#include "buffered_input.h"
#include "symbol_table.h"

#include <string>
#include <cstring>
#include <vector>
#include <iostream>

Token t; // Used so call functions can see the current token being matched against

/*
Purpose: Used to setup every prior analyzer.

Parameters: Takes a string filename of the file being parsed
Returns: An error if file is incorrect.
*/
Error parser_setup(std::string filename) {
    return lexical_setup(filename);
}

/*
Purpose: Starter function to call the recursive function.

Parameters: A root node to start parsing on, and the current token.
Returns: An error found while building the tree.
*/
Error parse(Node *&p, Token &token) {
    Error e;
    t = token;

    if (is_eof_token(e, t)) {
        return e;
    }

    p->id = STMT_BLOCK;
    while (e.id != ERR_EOF) {
        // Create new statement child node
        Node* child = new Node();
        
        e = stmt_level(child);
        //p->children.push_back(child);
        if (e.id != ERR_EOF && e.id != ERR_OK) {
            throw_error(e);
            return e;
        } 
        p->children.push_back(child);

        is_eof_token(e, t);
    }

    return e;
}

Error stmt_level(Node *&p) {
    Error e;

    if (t.id == IDENT) {
        if (t.value == "print") {
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e;

            // Opening print statement
            if (t.id == LPAREN) {
                p->id = PRINT;
            
                do {
                    // Print statment body
                    e = get_token(t);
                    if (e.id == ERR_EOF)
                        return e;

                    Node * child = new Node();
                    log_or_level(child);
                    p->children.push_back(child);
                } while (t.id == COMMA);

                // Closing print statement
                if (t.id == RPAREN) {
                    e = get_token(t);
                    if (e.id == ERR_EOF)
                        return e;
                    // Expect semicolon to finish statement
                    if (t.id != SEMICOLON) {
                        e.id = EXPECTED_TOKEN;
                        e.err_string = ";";
                        return e;
                    }
                    else {
                        e = get_token(t);
                        if (e.id == ERR_EOF)
                            return e;
                    }
                }
                // Expect parenthesis to close print function
                else {
                    e.id = EXPECTED_TOKEN;
                    e.err_string = ")";
                    return e;
                }
            }
            // Expect parenthesis to start function
            else {
                e.id = EXPECTED_TOKEN;
                e.err_string = "(";
                return e;
            }
        }
        // Read statement
        else if (t.value == "read") {
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e;

            if (t.id == LPAREN) {
                p->id = READ;

                // Get to variable to read into
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e;

                // TODO get the user input and assign symbol for variable or something
                if (symbol_search(t.value, SYMBOL_VAR) != -1) {
                    Node* child = new Node();
                    child->id = VARIABLE;
                    child->string_data = t.value;
                    p->left = child;
                }
                
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e;

                // Finish off the read statement
                if (t.id == RPAREN) {
                    e = get_token(t);
                    if (e.id == ERR_EOF)
                        return e;
                    if (t.id == SEMICOLON) {
                        e = get_token(t);
                        if (e.id == ERR_EOF)
                            return e;
                    }
                }
            }
        }
        // Variable declaration statement
        else if (t.value == "int4") {
            p->id = DECLARE_NODE;

            // Now should be on start of variable name
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e;

            
            std::string symbolName = t.value;
            p->string_data = symbolName;
            if (symbol_search(symbolName, SYMBOL_VAR) == -1)
                symbol_insert(symbolName, SYMBOL_VAR, SYMBOL_MEMORY);
            else {
                e.id = DUPLICATE_DECLARE;
                e.err_string = p->string_data;
                delete p;
                return e;
            }

            // Now should be on semicolon
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e;
            if (t.id == SEMICOLON) {
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e;
            }
            else {
                e.err_string = ";";
                e.id = EXPECTED_TOKEN;
                return e;
            }
        }
        // Variable assignment statement
        else if (symbol_search(t.value, SYMBOL_VAR) != -1) {
            p->id = ASSIGN_NODE;

            Node* var = new Node();
            var->type = INT4; // For now
            var->id = VARIABLE;
            var->string_data = t.value;

            e = get_token(t);
            if (e.id == ERR_EOF) {
                return e;
            }
            if (t.id != ASSIGN) {
                std::cout << "Error: Not an assign";
                delete var;
                // TODO Error
                return e;
            }

            // Start at expression token
            e = get_token(t);
            if (e.id == ERR_EOF) {
                delete var;
                return e;
            }

            Node* expr = new Node();
            addsub_level(expr);

            if (expr->type == INT4) {
                p->left = var;
                p->right = expr;
            }
            else {
                // TODO Error
                std::cout << "Variable assignment doesn't allow non-int4s\n";
                delete var;
                delete expr;
                return e;
            }

            if (t.id == SEMICOLON) {
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e;
            }
        }
        // If statement
        else if (t.value == "if") {
            p->id = IF_NODE;

            // Left parenthesis
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e; 
            if (t.id != LPAREN) {
                e.id = EXPECTED_TOKEN;
                e.err_string = "(";
                return e;
            }
            buffer_preceed();

            // Get condition statement
            e = get_token(t);
            //std::cout << t.id << std::endl;
            if (e.id == ERR_EOF)
                return e; 

            Node * child = new Node();
            e = log_or_level(child);
            if (e.id != ERR_EOF && e.id != ERR_OK) 
                return e;
            p->children.push_back(child); 

            // Right parenthesis
            //e = get_token(t);
            //if (e.id == ERR_EOF)
            //    return e; 
            //if (t.id != RPAREN) {
            //    e.id = EXPECTED_TOKEN;
            //    e.err_string = ")";
            //    return e;
            //}

            // Optional curly brace
            //e = get_token(t);
            //std::cout << "Optional thing: " << t.value << "\n";
            //if (e.id == ERR_EOF)
            //    return e; 

            if (t.id == LBRACE) {
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e; 

                Node * child = new Node();
                child->id = STMT_BLOCK;
                p->children.push_back(child);

                // Statement block
                while (t.id != RBRACE) {
                    Node * superChild = new Node();
                    e = stmt_level(superChild);
                    if (e.id != ERR_OK && e.id != ERR_EOF)
                        return e;
                    child->children.push_back(superChild); 
                }
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e; 
            }
            // Single statement
            else {
                Node * child = new Node();
                e = stmt_level(child);
                if (e.id != ERR_OK && e.id != ERR_EOF)
                    return e;
                p->children.push_back(child); 
            }
            if (t.value == "else") {
                // Optional curly brace
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e; 

                if (t.id == LBRACE) {
                    e = get_token(t);
                    if (e.id == ERR_EOF)
                        return e; 

                    Node * child = new Node();
                    child->id = STMT_BLOCK;
                    p->children.push_back(child);

                    // Statement block
                    while (t.id != RBRACE) {
                        Node * superChild = new Node();
                        e = stmt_level(superChild);
                        if (e.id != ERR_OK && e.id != ERR_EOF)
                            return e;
                        child->children.push_back(superChild); 
                    }
                    e = get_token(t);
                    if (e.id == ERR_EOF)
                        return e; 
                } 
                // Single statement
                else {
                    Node * child = new Node();
                    e = stmt_level(child);
                    if (e.id != ERR_OK && e.id != ERR_EOF)
                        return e;
                    p->children.push_back(child); 
                }
            }
        }
        // While statement
        else if (t.value == "while") {
            p->id = WHILE_NODE;

            // Left parenthesis
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e; 
            if (t.id != LPAREN) {
                e.id = EXPECTED_TOKEN;
                e.err_string = "(";
                return e;
            }
            buffer_preceed();

            // Get condition statement
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e; 

            Node * child = new Node();
            e = log_or_level(child);
            if (e.id != ERR_EOF && e.id != ERR_OK) 
                return e; 
            p->children.push_back(child); 

            // Right parenthesis
            //e = get_token(t);
            //if (e.id == ERR_EOF)
            //    return e; 
            //if (t.id != RPAREN) {
            //    e.id = EXPECTED_TOKEN;
            //    e.err_string = ")";
            //}

            // Optional curly brace
            //e = get_token(t);
            //if (e.id == ERR_EOF)
            //    return e; 

            if (t.id == LBRACE) {
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e; 

                Node * child = new Node();
                child->id = STMT_BLOCK;
                p->children.push_back(child);

                // Statement block
                while (t.id != RBRACE) {
                    Node * superChild = new Node();
                    e = stmt_level(superChild);
                    if (e.id != ERR_EOF && e.id != ERR_OK) 
                        return e; 
                    child->children.push_back(superChild); 
                }
                e = get_token(t);
                if (e.id == ERR_EOF)
                    return e; 
            } 
            // Single statement
            else {
                Node * child = new Node();
                e = stmt_level(child);
                if (e.id != ERR_EOF && e.id != ERR_OK) 
                    return e; 
                p->children.push_back(child); 
            } 
        }
        else if (t.value == "else") {
            e.id = EXPECTED_TOKEN;
            e.err_string = "Not else";
            return e;
        }
    }
    else {
        e = get_token(t);
        return e;
    }
    return e;
}


Error log_or_level(Node *&p) {
    Error e;
    // Go to next level
    e = log_and_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e;

    // Then check if it is |
    while (t.id == OR) {
        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;

        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = log_and_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        }
        
        p->type = BOOLEAN; 

        // Sets parent node
        p->right = rightNode;
        p->left = leftNode;
        p->string_data = "|";
        p->id = OR_NODE;

        if (e.id != ERR_OK && e.id != ERR_EOF)
            return e; 
    }
    return e; 
}


Error log_and_level(Node *&p) {
    Error e;
    // Go to next level
    e = log_not_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e;

    // Then check if it is &
    while (t.id == AND) {
        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;

        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = log_not_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        }
        
        p->type = BOOLEAN;

        // Sets parent node
        p->right = rightNode;
        p->left = leftNode;
        p->string_data = "&";
        p->id = AND_NODE;

        if (e.id != ERR_OK && e.id != ERR_EOF)
            return e; 
    }
    return e; 
}


Error log_not_level(Node *&p) {
    Error e;
    // Go to next level
    e = relative_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e;

    // Then check if it is !
    while (t.id == NOT) {
        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;

        // Make new node, right node
        Node *rightNode = new Node();
        e = relative_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete rightNode;
            return e;
        }
        
        p->type = BOOLEAN; 

        // Sets parent node
        p->right = rightNode;
        p->string_data = "not";
        p->id = NOT_NODE;

        if (e.id != ERR_OK && e.id != ERR_EOF)
            return e; 
    }
    
    return e; 
}


Error relative_level(Node *&p) {
    Error e;

    e = addsub_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e; 

    while (t.id == LESS || t.id == LESS_EQ || t.id == GREATER || t.id == GREATER_EQ || t.id == EQUAL || t.id == NOT_EQUAL) {
        std::string op;
        Node_ID id;
        if (t.id == LESS) {
            op = "<";
            id = LESS_NODE;
        }
        else if (t.id == LESS_EQ) {
            op = "<=";
            id = LESS_EQ_NODE;
        }
        else if (t.id == GREATER) {
            op = ">";
            id = GREATER_NODE;
        }
        else if (t.id == GREATER_EQ) {
            op = ">=";
            id = GREATER_EQ_NODE;
        }
        else if (t.id == EQUAL) {
            op = "==";
            id = EQ_NODE;
        }
        else {
            op = "~=";
            id = NOT_EQ_NODE;
        }

        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;

        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = addsub_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        }
        
        if (leftNode->type != INT4 || rightNode->type != INT4) {
            std::cout << "Error: NOT both ints";
            p->type = NO_TYPE;
            return e; // TODO finish error
        }
        
        p->type = BOOLEAN;

        // Sets parent node
        p->right = rightNode;
        p->left = leftNode;
        p->string_data = op;
        p->id = id;

        if (e.id != ERR_OK && e.id != ERR_EOF)
            return e; 
    }
    return e;
}

/*
Purpose: If token is a plus or minus, then evaluate both its operands,
          and create two new nodes as it's children.

Parameters: The current node to create a (sub)tree from.
Returns: An error found along the way.
*/
Error addsub_level(Node *&p) {
    Error e;
    // Go to next level
    e = multdiv_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e;

    // Then check if it is +/-
    while (t.id == PLUS || t.id == MINUS) {
        std::string op;
        Node_ID id;
        if (t.id == PLUS) {
            id = ADD;
            op = "+";
        }
        else {
            id = SUBTRACT;
            op = "-";
        }

        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;

        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = multdiv_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        }
        
        if (leftNode->type == INT4 && rightNode->type == INT4)
            p->type = INT4;
        else
            p->type = NO_TYPE; 

        // Sets parent node
        p->right = rightNode;
        p->left = leftNode;
        p->string_data = op;
        p->id = id;

        if (e.id != ERR_OK && e.id != ERR_EOF)
            return e; 
    }
    return e;
}

/*
Purpose: If token is a times or divide or mod, then evaluate both its operands,
          and create two new nodes as it's children.

Parameters: The current node to create a (sub)tree from.
Returns: An error found along the way.
*/
Error multdiv_level(Node *&p) {
    Error e;
    e = exp_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e; 

    // Then check if it is *//
    while (t.id == MULT || t.id == DIV || (t.id == IDENT && t.value == "mod")) {
        std::string op;
        Node_ID id;
        if (t.id == MULT) {
            id = MULTIPLY;
            op = "*";
        }
        else if (t.id == DIV) {
            id = DIVIDE;
            op = "/";
        }
        else {
            id = MOD;
            op = "mod";
        }

        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;
        
        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = exp_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        } 

        if (leftNode->type == INT4 && rightNode->type == INT4) {
            p->type = INT4;
        }
        else {
            p->type = NO_TYPE;
        }

        p->right = rightNode;
        p->left = leftNode; 
        p->string_data = op; 
        p->id = id;

        if (e.id != ERR_OK)
            return e;
    }

    return e;
}

/*
Purpose: If token is a power, then evaluate both its operands,
          and create two new nodes as it's children.

Parameters: The current node to create a (sub)tree from.
Returns: An error found along the way.
*/
Error exp_level(Node *&p) {
    Error e;
    e = data_level(p);
    if (e.id != ERR_OK && e.id != ERR_EOF)
        return e;
 
    if (t.id == EXP) {
        e = get_token(t);
        if (is_eof_token(e, t))
            return e;

        // Make new node, left node
        Node* leftNode = new Node();
        leftNode->left = p->left;
        leftNode->right = p->right;
        leftNode->int_data = p->int_data;
        leftNode->string_data = p->string_data;
        leftNode->id = p->id;
        leftNode->type = p->type;

        // Make new node, right node
        Node *rightNode = new Node();
        e = exp_level(rightNode);
        if (e.id != ERR_OK && e.id != ERR_EOF) {
            delete leftNode;
            delete rightNode;
            return e;
        } 
        
        if (leftNode->type == INT4 && rightNode->type == INT4)
            p->type = INT4;
        else
            p->type = NO_TYPE;

        p->right = rightNode;
        p->left = leftNode;
        p->string_data = "^"; 
        p->id = POWER;

        if (e.id != ERR_OK)
            return e; 
    }
    return e;
}

/*
Purpose: If token is a unary operator, then get its new node operand,
          and if not check, check for parenthesis and raw data to get.

Parameters: The current node to create a (sub)tree from.
Returns: An error found along the way.
*/
Error data_level(Node *&p) {
    Error e;
    // If token is a variable/value
    if (t.id == INTEGER || t.id == IDENT || t.id == STRING) {
        if (t.id == INTEGER) {
            p->id = VALUE;
            p->int_data = t.int_value;
            p->type = INT4;
        }
        else if (t.id == STRING) {
            p->id = STR_ID;
            p->type = STR_CONST;
            p->string_data = t.value;

            char* cstr = new char[t.value.length()+1];
            std::strcpy(cstr, t.value.c_str());

            p->int_data = store_string(cstr);
        }
        else if (t.id == IDENT && (t.value == "true" || t.value == "false")) {
            p->id = BOOL;
            p->type = BOOLEAN;
            if (t.value == "true") {
                p->string_data = "true";
            }
            else {
                p->string_data = "false";
            }
        }
        else if (t.id == IDENT) {
            if (symbol_search(t.value, SYMBOL_VAR) != -1) {
                p->id = VARIABLE; // Only store int4 for now vvv
                p->type = INT4;
                p->string_data = t.value;
            }
            else {
                // TODO return error
                return e;
            }
        }
        e = get_token(t);
        if (is_eof_token(e, t))
            return e;
    }
    // If token is the start of a parenthesis
    else if (t.id == LPAREN) {
        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;
        log_or_level(p);

        // Look for the closing parenthesis
        if (t.id == RPAREN) {
            e = get_token(t);
            if (is_eof_token(e, t)) {
                return e;
            }
        }
        else {
            e = get_token(t);
            if (e.id == ERR_EOF)
                return e;
            e.id = EXPECTED_TOKEN;
            e.err_string = ")";
            return e;
        }
    }
    // If token is a unary operator, signing a value
    else if (t.id == PLUS || t.id == MINUS) {
        std::string op;
        if (t.id == PLUS) {
            op = "+";
        }
        else {
            op = "-";
        }
        
        e = get_token(t);
        if (e.id == ERR_EOF)
            return e;
        if (t.id == MINUS || t.id == PLUS) {
            if (t.id == MINUS)
                e.err_string = "-";
            else
                e.err_string = "+";
            e.id = UNKNOWN_TOKEN;
            e.line_number = t.line_number;
            e.column_number = t.column_number;

            std::string line;
            buffer_line_contents(t.line_number, line);
            std::cout << line;
            for (int i = 0; i < t.column_number - 1; i++)
                std::cout << "-";
            std::cout << "^\n";
            throw_error(e);

            e = get_token(t);
            if (is_eof_token(e, t))
                return e;

            std::cout << "C: " << t.column_number;
            e.id = UNKNOWN_TOKEN;
            return e;
        }

        // Make new node, right node
        Node *rightNode = new Node();
        p->right = rightNode;
        e = data_level(p->right);
        /*
        if (e.id != ERR_OK)
            return e;
            */
        
        if (op == "+")
            p->id = POS;
        else
            p->id = NEG;
        p->string_data = op;
        p->type = INT4;

        if (e.id != ERR_OK)
            return e;
    }
    else if (t.id == NOT)
        return e;
    else {
        e.id = UNKNOWN_TOKEN;
        throw_error(e);
        get_token(t);
        if (is_eof_token(e, t))
            return e;
    }
    return e;
}

/*
Purpose: To see if eof has been hit.

Returns: Whether or not the end of file has been reached.
*/
bool is_eof() {
    return buffer_eof();
}

/*
Purpose: Recursively deletes all the nodes in the tree (postorder)

Parameters: The current node to act on.
*/
void parser_cleanup(Node *&p) {
    if (p == nullptr)
        return;

    parser_cleanup(p->left);
    parser_cleanup(p->right);

    delete p;
}

