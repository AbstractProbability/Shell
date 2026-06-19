#ifndef CD_H
#define CD_H

#include "common.h"
#include "parser.h"

void update_dirs(char **curr_dir, char **prev_dir);

void callcd(arg_node *args_head, char **p_parent_dir, char **p_curr_dir, char **p_prev_dir);

#endif

