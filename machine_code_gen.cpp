/*
CSCI 490 Assign 2
By: Aaron Carreon (z1957830)
Due: 2/25/2025

Purpose: Converts a parse tree of arithmetic expressions into machine code to
          compute values from.
*/ 
#include "machine_code_gen.h"
#include "symbol_table.h"
#include "storage.h"
#include "node.h"
#include "disasm.h"

#include <iostream>
#include <sys/mman.h>
#include <errno.h>

using std::cout;
using std::cin;
using std::endl;

const int PROG_SIZE = 50000;
unsigned char * prog;
int p_offset;

/*
Purpose: Sets the memory to run instructions on, reset 'instruction' counter.
*/
void machine_setup() {
    prog = (unsigned char *) mmap(0, PROG_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (errno) {
        perror("mmap");
        return;
    }
    p_offset = 0;
}

/*
Purpose: unmaps the memory used for instructions
*/
void machine_cleanup() {
    munmap(prog, PROG_SIZE);
}

/*
Purpose: start the recursive call of instruction forming, then adds return statement.

Parameters: Takes a root node of a parse tree.
*/
void generate_instructions(Node *&n) {
    parse_tree(n);

    prog[p_offset++] = 0xc3; // RET

    //disassemble((unsigned char *)prog, p_offset);
    // For testing (hex dump)
    //for (int i = 0; i < p_offset; i++) 
    //    cout << std::hex << int(prog[i]) << " ";  
}

/*
Purpose: performs a postorder traversal to compute values in nodes in a tree.

Parameters: Takes a root node of a parse (sub)tree.
*/
void parse_tree(Node *&n) {
    if (n == nullptr)
        return;
    // Post order traversal of tree, adding stuff to stack, then popping two once finding an operator (or one for unary)
   
    if (!n->children.empty()) {
        // If print statement, print out all expressions
        if (n->id == PRINT) {
            for (long unsigned int i = 0; i < n->children.size(); i++) {
                parse_tree(n->children[i]);
                // Node is a int4, variable or explicit
                if (n->children[i]->type == INT4) {
                    prog[p_offset++] = 0x48; // MOV RSI, print_int4
                    prog[p_offset++] = 0xbe; 
                    add_int8((long long int) print_int4);
                    prog[p_offset++] = 0x89; // MOVE EDI, EAX
                    prog[p_offset++] = 0xc7;
                }
                else if (n->children[i]->type == STR_CONST) {
                    prog[p_offset++] = 0x48; // MOV RSI, print_string
                    prog[p_offset++] = 0xbe; 
                    add_int8((long long int) print_string);
                    prog[p_offset++] = 0x48; // MOVE RDI, RAX
                    prog[p_offset++] = 0x89;
                    prog[p_offset++] = 0xc7;
                }
                else if (n->children[i]->type == BOOLEAN) {
                    prog[p_offset++] = 0x48; // MOV RSI, print_bool
                    prog[p_offset++] = 0xbe; 
                    add_int8((long long int) print_bool);
                    prog[p_offset++] = 0x89; // MOV EDI, AL
                    prog[p_offset++] = 0xc7;
                }
                prog[p_offset++] = 0xff; // CALL RSI
                prog[p_offset++] = 0xd6;
                //prog[p_offset++] = 0xc3; // RET
            }
        }
        else if (n->id == IF_NODE) {
            // Generate the code for the logical condition
            parse_tree(n->children[0]);
            prog[p_offset++] = 0xa8; //  TST AL, 1 (immediate)
            prog[p_offset++] = 0x01;
            prog[p_offset++] = 0x0f; // JZ (32-bit)
            prog[p_offset++] = 0x84;

            int size = 0;
            get_tree_byte_size(n->children[1], size);
            // If there's an else, jump over the non-conditional jump as well
            if (n->children.size() > 2)
                size += 5;
            add_int4(size); 

            // Generate code of if body
            parse_tree(n->children[1]);

            // With an else clause, generate nonconditional jump
            if (n->children.size() > 2) {
                prog[p_offset++] = 0xe9;

                size = 0;
                get_tree_byte_size(n->children[2], size);
                add_int4(size);

                // Now generate code of else block
                parse_tree(n->children[2]);
            }
        }
        else if (n->id == WHILE_NODE) {
            // Generate the code for a nonconditional jump to logical condition
            prog[p_offset++] = 0xe9;
            int size = 0;
            get_tree_byte_size(n->children[1], size);
            add_int4(size);

            // Generate code for body of loop
            parse_tree(n->children[1]);

            // Generate code for logical condition
            parse_tree(n->children[0]);

            // Generate TST code
            prog[p_offset++] = 0xa8; // TST AL, 1
            prog[p_offset++] = 0x01;

            // Jump to code for body of loop if condition is true
            prog[p_offset++] = 0x0f;
            prog[p_offset++] = 0x85;
            int logSize = 0;
            get_tree_byte_size(n->children[0], logSize);
            add_int4(-1 * (logSize + size + 8));
        }
        else {
            for (long unsigned int i = 0; i < n->children.size(); i++) {
                parse_tree(n->children[i]);
            }
        }
    }

    parse_tree(n->left);

    if (n->id == ADD || n->id == SUBTRACT || n->id == MULTIPLY || n->id == DIVIDE || n->id == MOD || n->id == POWER || n->id == LESS_NODE || n->id == LESS_EQ_NODE || n->id == GREATER_NODE || n->id == GREATER_EQ_NODE || n->id == EQ_NODE || n->id == NOT_EQ_NODE) {
        prog[p_offset++] = 0x50; // PUSH EAX
    }
    else if (n->id == OR_NODE) { // generated left subtree in AL (because true or false)
        prog[p_offset++] = 0xa8; //  TEST AL, 1 (immediate)
        prog[p_offset++] = 0x01;
        prog[p_offset++] = 0x0f; // JNZ (32-bit)
        prog[p_offset++] = 0x85;

        int size = 0;
        get_tree_byte_size(n->right, size);
        add_int4(size);
    }
    else if (n->id == AND_NODE) {
        prog[p_offset++] = 0xa8; //  TEST AL, 1 (immediate)
        prog[p_offset++] = 0x01;
        prog[p_offset++] = 0x0f; // JZ (32-bit)
        prog[p_offset++] = 0x84;

        int size = 0;
        get_tree_byte_size(n->right, size);
        add_int4(size); 
    }

    parse_tree(n->right);
    
    
    if (n->id == VALUE) {
        prog[p_offset++] = 0xb8; // MOV EAX, VALUE (move value into accumulator register)
        
        // put the other (32 bits/4 bytes into memory)
        int value = n->int_data;
        // Put rightmost byte first, then rest of the 3 integer bytes
        prog[p_offset++] = value & 0xff;
        prog[p_offset++] = (value >> 8) & 0xff;
        prog[p_offset++] = (value >> 16) & 0xff;
        prog[p_offset++] = (value >> 24) & 0xff;
    }
    else if (n->id == STR_ID) {
        // Just in case, reset RAX
        prog[p_offset++] = 0x48; // XOR RAX, RAX
        prog[p_offset++] = 0x31;
        prog[p_offset++] = 0xc0;
        prog[p_offset++] = 0x48; // MOV RAX, STR_ID string_data
        prog[p_offset++] = 0xb8;

        // Retrieve from storage
        add_int8((long long int) retrieve_string(n->int_data));
    }
    else if (n->id == VARIABLE) {
        // TODO can probably save bytes here, gets called on read and then again when it doesn't need to and such.
        int index = symbol_search(n->string_data, SYMBOL_VAR);
        if (index != -1) {
            prog[p_offset++] = 0x48; // MOV RSI, &int4_storage
            prog[p_offset++] = 0xbe;
            add_int8((long long int) get_int4_table());
            prog[p_offset++] = 0x8b; // MOV EAX, [ESI] * index * 4
            prog[p_offset++] = 0x86;
            add_int4(index * 4);
        } 
    }
    else if (n->id == BOOL) {
        // Set ax to 0 if false
        prog[p_offset++] = 0x31; // XOR EAX, EAX
        prog[p_offset++] = 0xc0;

        // Increment by 1 if true
        if (n->string_data == "true") {
            prog[p_offset++] = 0xff; // INC EAX // TODO turn into AL
            prog[p_offset++] = 0xc0;
        }
    }
    else if (n->id == DECLARE_NODE) {
        prog[p_offset++] = 0x31; // XOR EAX, EAX
        prog[p_offset++] = 0xc0;
        move_symbol_eax(n);
    } 
    else if (n->id == ASSIGN_NODE) {
        // Value now in machine code EAX
        int index = symbol_search(n->left->string_data, SYMBOL_VAR);
        if (index != -1) {
            prog[p_offset++] = 0x48; // MOV RSI, &int4_storage
            prog[p_offset++] = 0xbe;
            add_int8((long long int) get_int4_table());
            // Ideally After this is only 4 bytes, is now 10
            prog[p_offset++] = 0x89; // MOV [RSI + index * 4], EAX
            prog[p_offset++] = 0x86;
            add_int4(index * 4);
        }
    }
    else if (n->id == READ) {
        // Left child is variable
        prog[p_offset++] = 0x48; // MOV RSI, read_int4
        prog[p_offset++] = 0xbe;
        add_int8((long long int) read_int4);
        prog[p_offset++] = 0xff; // CALL RSI
        prog[p_offset++] = 0xd6;
        move_symbol_eax(n->left);
    }
    else if (n->id == ADD) {
        prog[p_offset++] = 0x59; // POP ECX

        prog[p_offset++] = 0x03; // ADD EAX, ECX
        prog[p_offset++] = 0xc1;
    }
    else if (n->id == SUBTRACT) {
        prog[p_offset++] = 0x59; // POP ECX

        prog[p_offset++] = 0x91; // XCHG EAX, ECX
        prog[p_offset++] = 0x29; // SUB EAX, ECX
        prog[p_offset++] = 0xc8;
    }
    else if (n->id == NEG) {
        prog[p_offset++] = 0xf7; // NEG EAX
        prog[p_offset++] = 0xd8;
    }
    else if (n->id == MULTIPLY) {
        prog[p_offset++] = 0x59; // POP ECX

        prog[p_offset++] = 0xf7; // IMULT (EAX,) ECX
        prog[p_offset++] = 0xe9;
    }
    else if (n->id == DIVIDE || n->id == MOD) {
        prog[p_offset++] = 0x59; // POP ECX

        prog[p_offset++] = 0x91; // XCHG

        prog[p_offset++] = 0x99; // CDQ
        prog[p_offset++] = 0xf7; // IDIV (EAX,DX pair) ECX
        prog[p_offset++] = 0xf9; //  quotient in accumulator

        if (n->id == MOD) {
            prog[p_offset++] = 0x89; // MOV EAX, EDX
            prog[p_offset++] = 0xd0;
        }
    }
    else if (n->id == POWER) {
        prog[p_offset++] = 0x50; // PUSH EAX

        prog[p_offset++] = 0x45; // XOR R8, R8 // e = 0
        prog[p_offset++] = 0x31;
        prog[p_offset++] = 0xc0;
        prog[p_offset++] = 0x41; // POP R9     // get a
        prog[p_offset++] = 0x8f;
        prog[p_offset++] = 0xc2;
        prog[p_offset++] = 0x41; // POP R10    // get b
        prog[p_offset++] = 0x8f;
        prog[p_offset++] = 0xc1;

        prog[p_offset++] = 0x45; // TEST R10, R10 // if (b < 0)
        prog[p_offset++] = 0x85;
        prog[p_offset++] = 0xd2;
        prog[p_offset++] = 0x7c; // JL         // if (b < 0> jump to end
        prog[p_offset++] = 0x1e;
        prog[p_offset++] = 0x41; // INC R8     // e = 1
        prog[p_offset++] = 0xff;
        prog[p_offset++] = 0xc0;
        prog[p_offset++] = 0x45; // TEST R10, R10 // start of while loop
        prog[p_offset++] = 0x85;
        prog[p_offset++] = 0xd2;
        prog[p_offset++] = 0x74; // JE         // if (b = 0) exit loop,
        prog[p_offset++] = 0x16;
        prog[p_offset++] = 0x41; // TEST R10, 1 // if (b & 1)
        prog[p_offset++] = 0xf7;
        prog[p_offset++] = 0xc2;
        prog[p_offset++] = 0x01;
        prog[p_offset++] = 0x00;
        prog[p_offset++] = 0x00;
        prog[p_offset++] = 0x00;
        prog[p_offset++] = 0x74; // JE if (b & 1) is 0, jump over next IMUL
        prog[p_offset++] = 0x04;
        prog[p_offset++] = 0x45; // IMUL R8, R9 // e = e * a
        prog[p_offset++] = 0x0f;
        prog[p_offset++] = 0xaf;
        prog[p_offset++] = 0xc1;
        prog[p_offset++] = 0x45; // IMUL R9, R9 // a = a * a
        prog[p_offset++] = 0x0f;
        prog[p_offset++] = 0xaf;
        prog[p_offset++] = 0xc9;
        prog[p_offset++] = 0x41; // SAR R10, 1  // b >>= 1
        prog[p_offset++] = 0xd1;
        prog[p_offset++] = 0xfa;
        prog[p_offset++] = 0xeb; // JMP     // back to start of while loop
        prog[p_offset++] = 0xe5;
        prog[p_offset++] = 0x41; // MOV EAX, R8 // move result (e = a^b) to EAX
        prog[p_offset++] = 0x8b;
        prog[p_offset++] = 0xc0;
    }
    else if (n->id == LESS_NODE || n->id == LESS_EQ_NODE || n->id == GREATER_NODE || n->id == GREATER_EQ_NODE || n->id == EQ_NODE || n->id == NOT_EQ_NODE) {
        prog[p_offset++] = 0x59; // POP ECX

        prog[p_offset++] = 0x39; // CMP EAX, ECX
        prog[p_offset++] = 0xc1;
        prog[p_offset++] = 0x0f; // SETB AL
        if (n->id == LESS_NODE)
            prog[p_offset++] = 0x9c;
        else if (n->id == LESS_EQ_NODE)
            prog[p_offset++] = 0x9e;
        else if (n->id == GREATER_NODE)
            prog[p_offset++] = 0x9f; 
        else if (n->id == GREATER_EQ_NODE)
            prog[p_offset++] = 0x9d; 
        else if (n->id == EQ_NODE)
            prog[p_offset++] = 0x94; 
        else
            prog[p_offset++] = 0x95;
        prog[p_offset++] = 0xc0;
    }
    else if (n->id == NOT_NODE) {
        prog[p_offset++] = 0xf6; // NOT AL
        prog[p_offset++] = 0xd0;
    }
}

/*
Purpose: Calls the instructions generated and mapped onto our allocated memory
*/
void execute_instructions() {
    cout << "Code size: " << p_offset << " bytes.\n";
    cout << "Code execution:\n";

    ((int (*)(void)) prog) ();

    cout << endl;
}


void print_int4(int i) {
    cout << i;
}


void print_string(char *c) {
    cout << c;
}


void print_bool(int b) {
    // Isolate the rightmost bit
    if ((b << 31) == 0)
        cout << "false";
    else
        cout << "true";
}


int read_int4() {
    int i;
    cin >> i;

    return i;
}


void add_int4(int value) {
    // Put rightmost byte first, then rest of the 3 integer bytes
    prog[p_offset++] = value & 0xff;
    prog[p_offset++] = (value >> 8) & 0xff;
    prog[p_offset++] = (value >> 16) & 0xff;
    prog[p_offset++] = (value >> 24) & 0xff; 
}


void add_int8(long long int value) {
    // Little-endian
    // Put rightmost byte first, then rest of the 7 integer bytes
    prog[p_offset++] = value & 0xff;
    prog[p_offset++] = (value >> 8) & 0xff;
    prog[p_offset++] = (value >> 16) & 0xff;
    prog[p_offset++] = (value >> 24) & 0xff;
    prog[p_offset++] = (value >> 32) & 0xff; 
    prog[p_offset++] = (value >> 40) & 0xff; 
    prog[p_offset++] = (value >> 48) & 0xff; 
    prog[p_offset++] = (value >> 56) & 0xff; 
}


void move_symbol_eax(Node *n) {
    int index = symbol_search(n->string_data, SYMBOL_VAR);
    if (index != -1) {
        prog[p_offset++] = 0x48; // MOV RSI, &int4_storage
        prog[p_offset++] = 0xbe;
        add_int8((long long int) get_int4_table());
        // Ideally After this is only 4 bytes, is now 10
        //prog[p_offset++] = 0xc7;
        prog[p_offset++] = 0x89;
        prog[p_offset++] = 0x86;
        add_int4(index * 4);
        //add_int4(value); 
    }
}


void get_tree_byte_size(Node *p, int &size) {
    if (p == nullptr)
        return;

    switch(p->id) {
        case VARIABLE:
            size += 16;
            break;
        case STR_ID:
            size += 13;
            break; 
        case VALUE:
            size += 5;
            break;
        case ADD:
            size += 4;
            break;
        case SUBTRACT:
            size += 5;
            break; 
        case MULTIPLY:
            size += 4;
            break; 
        case DIVIDE:
            size += 6;
            break;
        case MOD:
            size += 8;
            break; 
        case POWER:
            size += 49;
            break; 
        case NEG:
            size += 2;
            break;
        case STMT_BLOCK:
            for (long unsigned int i = 0; i < p->children.size(); i++) {
                get_tree_byte_size(p->children[i], size);
            }
            break; 
        case PRINT:
            // Find size depending on type of child
            for (long unsigned int i = 0; i < p->children.size(); i++) {
                get_tree_byte_size(p->children[i], size);
                if (p->children[i]->type == STR_CONST) {
                    size += 15;
                }
                else {
                    size += 14;
                }
            }
            break;
        case READ:
            size += 28;
            break; 
        case ASSIGN_NODE:
            size += 16; // I think
            break; 
        case DECLARE_NODE:
            size += 18;
            break;
        case BOOL:
            size += 0; // TODO 2 or 4
            break; 
        case LESS_NODE:
            size += 7;
            break; 
        case LESS_EQ_NODE:
            size += 7;
            break;
        case GREATER_NODE:
            size += 7;
            break; 
        case GREATER_EQ_NODE:
            size += 7;
            break; 
        case EQ_NODE:
            size += 7;
            break;
        case NOT_EQ_NODE:
            size += 7;
            break; 
        case OR_NODE:
            size += 8;
            break; 
        case AND_NODE:
            size += 8;
            break;
        case NOT_NODE:
            size += 2;
            break;  
        case IF_NODE:
            for (long unsigned int i = 0; i < p->children.size(); i++) {
                get_tree_byte_size(p->children[i], size);
            }
            if (p->children.size() == 3) {
                size += 5;
            }
    
            size += 8;
            break;
        case WHILE_NODE:
            size += 13;
            for (long unsigned int i = 0; i < p->children.size(); i++) {
                get_tree_byte_size(p->children[i], size);
            }

            break;
        default:
            break;
    }

    get_tree_byte_size(p->left, size);
    get_tree_byte_size(p->right, size);
}
