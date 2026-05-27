#include "storage.h"

char* string_table[500];
int int4_table[500];

int string_offset = 0;
int int_offset = 0;

int store_string(char* s) {
    if (string_offset == 500)
        return -1;
    string_table[string_offset++] = s;

    return string_offset - 1;
}


int store_int4(int i) {
    if (int_offset == 500)
        return -1;
    int4_table[int_offset++] = i;

    return int_offset - 1;
}

char* retrieve_string(int index) {
    return string_table[index];
}

int retrieve_int4(int index) {
    return int4_table[index];
}

int* get_int4_table() {
    return int4_table;
}
