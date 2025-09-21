#include "../include/hop.h"

void update_dirs(char **p_current_directory, char **p_previous_directory) {
    free(*p_previous_directory);
    *p_previous_directory = *p_current_directory;

    char *cdir = malloc(MAX_CHARS * sizeof(char));
    getcwd(cdir, MAX_CHARS);

    *p_current_directory = cdir;
    
}

void hopp(arg_node *args_head, arg_node *args_tail, char **p_parent_directory, char **p_current_directory, char **p_previous_directory) {
    int arg_size = 0;
    arg_node *temp = args_head;
    while(temp != NULL) {
        arg_size++;
        temp = temp->next_arg;
    }

    char *parent_directory = *p_parent_directory, *previous_directory = *p_previous_directory;
    temp = args_head;

    if (arg_size == 0) {
        // change to home directory
        if (chdir(parent_directory)) return;
        update_dirs(p_current_directory, p_previous_directory);
    }
    while (temp != NULL) {
        if (strcmp("~", temp->command) == 0) 
        {
            // change to home directory
            if (chdir(parent_directory)) return;
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
            if (strcmp(previous_directory, "") == 0) {
                return;
            } else {
                if (chdir(previous_directory)) return;
            }
        }
        else
        {
            if (chdir(temp->command)) {
                fprintf(stdout, "No such directory!\n");
                return;
            }
        }
        update_dirs(p_current_directory, p_previous_directory);
        temp = temp->next_arg;
    }
    
}