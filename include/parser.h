#ifndef PARSER_H
#define PARSER_H

#define FILE_TRUNCATE   1
#define FILE_APPEND     2

typedef struct arg_node {
    char *arg;
    struct arg_node *next;
} arg_node;

typedef struct cmd_node {
    char *command;
    arg_node *args_head;             // linked list of args
    arg_node *args_tail;
    int argc;
    int pipe_in;
    int pipe_out;
    struct cmd_node *next;
} cmd_node;

typedef struct cmd_group_node {
    cmd_node *cmds_head;             // linked list of piped cmd_node
    cmd_node *cmds_tail;
    char *input_filename;
    char *output_filename;
    int has_input_file;
    int has_output_file;
    int is_background;          // boolean representing backround execution
    struct cmd_group_node *next;
} cmd_group_node;

char *get_next_token(char **p_input);

void print_ast(cmd_group_node *head);

cmd_group_node *build_ast(char *input_line);

#endif

