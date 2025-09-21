#ifndef reveal
#define reveal

#include "common.h"
#include "parser.h"

// list directory contents
void list_directory(int a, int l);

// main reveal call
void reveall(ast_node *head, char *current_directory, char *parent_directory, char *previous_directory) ;

#endif