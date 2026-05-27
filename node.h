/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Small node structure that holds a id, value and two pointers to children
          which forms the basis of a binary tree.
*/
#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

// List of different node ids
enum Node_ID {
    NO_ID,        // 0
    VARIABLE,
    STR_ID,
    VALUE,
    ADD,
    SUBTRACT,     // 5
    MULTIPLY,
    DIVIDE,
    MOD,
    POWER,
    NEG,          // 10
    POS,
    STMT_BLOCK,
    PRINT,
    READ,
    ASSIGN_NODE,  // 15
    DECLARE_NODE,
    BOOL,
    LESS_NODE,
    LESS_EQ_NODE,
    GREATER_NODE, // 20
    GREATER_EQ_NODE,
    EQ_NODE,
    NOT_EQ_NODE,
    OR_NODE,
    AND_NODE,     // 25
    NOT_NODE,
    IF_NODE,
    WHILE_NODE
};

enum Node_Type {
    NO_TYPE,
    INT4,
    STR_CONST,
    BOOLEAN
};

class Node {
    public:
        Node_ID id;
        Node_Type type;

        std::vector<Node*> children;
        Node* left;
        Node* right;

        std::string string_data;
        long int_data;

        // Constructors
        Node();
        Node(std::string data);
        Node(long data);
};

#endif /* NODE_H */
