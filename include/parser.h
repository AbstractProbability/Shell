#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include <ctype.h>

typedef struct token_list_node {
    char *token;
    struct token_list_node *next;
} token_list_node;

typedef struct arg_node {
    char *command;
    struct arg_node *next_arg;
} arg_node;

typedef struct ast_node {
    char *command;
    arg_node *command_args_head;
    char *input_filename;
    char *output_filename;
    int output_file_mode; // 0 = none, 1 = overwrite, 2 = append
    int pipe_send;
    int background;
    struct ast_node *next;
} ast_node;

// checks special char except whitespace
int is_special(char c);

// tokenise the input string
// by spaces and group special characters
token_list_node *tokenise(char *input_string);

// return valid ast, else grammar error
ast_node* build_ast(token_list_node *token_head);

#endif

