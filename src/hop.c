#include "../include/hop.h"

void update_dirs(char **p_curr_dir, char **p_prev_dir) {
    free(*p_prev_dir);
    *p_prev_dir = *p_curr_dir;

    char *cdir = malloc((MAX_CHARS+1) * sizeof(char));
    getcwd(cdir, MAX_CHARS);

    *p_curr_dir = cdir;

}

void hopp(arg_node *args_head, arg_node *args_tail, char **p_parent_dir, char **p_curr_dir, char **p_prev_dir) {
    int arg_size = 0;
    arg_node *temp = args_head;
    while(temp != NULL) {
        arg_size++;
        temp = temp->next_arg;
    }

    char *parent_dir = *p_parent_dir, *prev_dir = *p_prev_dir;
    temp = args_head;

    if (arg_size == 0) {
        // change to home directory
        if (chdir(parent_dir)) return;
        update_dirs(p_curr_dir, p_prev_dir);
    }
    while (temp != NULL) {
        if (strcmp("~", temp->command) == 0)
        {
            // change to home directory
            if (chdir(parent_dir)) return;
        }
        else if (strcmp(temp->command, ".") == 0)
        {
            return;
        }
        else if (strcmp(temp->command, "..") == 0)
        {
            if (chdir("..")) return;
        }
        else if (strcmp(temp->command, "-") == 0)
        {
            if (strcmp(prev_dir, "") == 0) {
                return;
            } else {
                if (chdir(prev_dir)) return;
            }
        }
        else
        {
            if (chdir(temp->command)) {
                fprintf(stdout, "No such directory!\n");
                return;
            }
        }
        update_dirs(p_curr_dir, p_prev_dir);
        temp = temp->next_arg;
    }

}

