#include "../include/reveal.h"

int cmpdir(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

// list directory contents
void list_directory(int a, int l) {
    struct dirent **namelist;
    /*if (l) {

    }*/
    int n = scandir(".", &namelist, NULL, cmpdir);

    for (int i = 0; i<n; i++) {
        if (!a) {
            if (namelist[i]->d_name[0] == '.')
                continue;
        }
        if (l)
            printf("%s\n", namelist[i]->d_name);
        else
            printf("%s  ", namelist[i]->d_name);
    }

    if (!l)
        printf("\n");
}

// main reveal call
void reveall(ast_node *head, char *curr_dir, char *parent_dir, char *prev_dir) {
    arg_node *temp = head->command_args_head;
    int l = 0, a = 0, bad = 0;
    while (temp != NULL && temp->command[0] == '-' && temp->command[1] != '\0') {
        int i = 1;
        
        while (temp->command[i] != '\0') {
            if (temp->command[i] != 'a' && temp->command[i] != 'l') {
                bad = 1;
                break;
            }
            i++;
        }
        if (bad) break;

        i = 1;
        while (temp->command[i] != '\0') {
            if (temp->command[i] == 'a')
                a = 1;
            else if (temp->command[i] == 'l')
                l = 1;
            else {
                fprintf(stderr, "reveal: invalid flags!\n");
                return;
            }
            i++;
        }
        temp = temp->next_arg;
    }

    arg_node *temp2 = temp;
    int i = 0;
    while (temp2 != NULL) {
        i++;
        temp2 = temp2->next_arg;
    }

    if (i > 1) {
        fprintf(stderr, "reveal: Invalid Syntax!\n");
        return;
    }

    int chdir_ret;
    if (i == 0 || strcmp(temp->command, ".") == 0)
    {
        // list current directory
        chdir_ret = chdir(curr_dir);
    }
    else 
    {
        if (strcmp(temp->command, "~") == 0)
        {
            // list parent directory
            chdir_ret = chdir(parent_dir);
        }
        else if (strcmp(temp->command, "-") == 0)
        {
            // list previous directory
            if (prev_dir[0] == '\0') {
                fprintf(stderr, "No such directory!\n");
                return;
            }
            chdir_ret = chdir(prev_dir);
        }
        else if (strcmp(temp->command, "..") == 0)
        {
            // list upper directory
            chdir_ret = chdir("..");
        }
        else 
        {
            chdir_ret = chdir(temp->command);
        }
    }
    
    if (chdir_ret) {
        fprintf(stderr, "No such directory!\n");
        chdir(curr_dir);
        return;
    }

    list_directory(a, l);
    chdir(curr_dir);
}