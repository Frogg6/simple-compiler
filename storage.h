#ifndef STORAGE_H
#define STORAGE_H

int store_string(char*);
int store_int4(int);

char* retrieve_string(int);
int retrieve_int4(int);

int* get_int4_table();

#endif /* STORAGE_H */
