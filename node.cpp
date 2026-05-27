/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Small node structure that holds a id, value and two pointers to children
          which forms the basis of a binary tree.
*/
#include "node.h"

#include <string>
#include <vector>

/*
Purpose: A default constructor, sets everything to nothing
*/
Node::Node() {
    id = NO_ID;
    type = NO_TYPE;

    children = std::vector<Node*>();
    left = nullptr;
    right = nullptr;

    string_data = "";
    int_data = 0;
}

/*
Purpose: Constructor that can be given starting string data.
*/
Node::Node(std::string data) {
    id = NO_ID;
    type = NO_TYPE;

    children = std::vector<Node*>();
    left = nullptr;
    right = nullptr;

    string_data = data;
    int_data = 0;
}

/*
Purpose: Constructor that can be given starting int data.
*/
Node::Node(long data) {
    id = NO_ID;
    type = NO_TYPE;

    children = std::vector<Node*>();
    left = nullptr;
    right = nullptr;

    int_data = data;
    std::string sting_data = "";
}

