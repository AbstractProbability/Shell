#ifndef PARSER2_H
#define PARSER2_H

#define FILE_TRUNCATE   1
#define FILE_APPEND     2

typedef struct arg_node {
    char *arg;
    struct arg_node *next;
} arg_node;

typdef struct cmd_node {
    char *command;
    int argc;
    arg_node *args;             // array of args
    char *input_filename;
    char *output_filename;
    int has_input_file;
    int has_output_file;
    int pipe_in;
    int pipe_out;
    struct cmd_node *next;
}

typedef struct cmd_group_node {
    cmd_node *cmds_head;             // linked list of piped cmd_node
    cmd_node *cmds_tail;
    int is_background;          // boolean representing backround execution
    struct cmd_group_node *next;
} cmd_group;

// checks special char except whitespace
int is_special(char c);

char *get_next_token(char **p_input);

cmd_group_node *build_ast(char *input_line);

#endif

