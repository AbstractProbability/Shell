#ifndef hop
#define hop

#include "common.h"
#include "parser.h"

void update_dirs(char **curr_dir, char **prev_dir);

void hopp(arg_node *args_head, arg_node *args_tail, char **p_parent_dir, char **p_curr_dir, char **p_prev_dir);

#endif