#ifndef REVEAL_H
#define REVEAL_H

#include "parser.h"

// list directory contents
void list_directory(int a, int l);

// main reveal call
void reveall(ast_node *head, char *curr_dir, char *parent_dir, char *prev_dir) ;

#endif

