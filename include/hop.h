#ifndef hop
#define hop

#include "common.h"
#include "parser.h"

void update_dirs(char **current_directory, char **previous_directory);

void hopp(arg_node *args_head, arg_node *args_tail, char **p_parent_directory, char **p_current_directory, char **p_previous_directory);

#endif