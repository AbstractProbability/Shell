#ifndef parser
#define parser

#include "common.h"
#include "tokeniser.h"

typedef struct arg_node {
    char *command;
    struct arg_node *next_arg;
} arg_node;

typedef struct ast_node {
    char *command;
    arg_node *command_args_head;
    arg_node *command_args_tail;
    char *input_filename;
    char *output_filename;
    int output_file_mode; // 0 = none, 1 = overwrite, 2 = append
    int female;
    int male;
    int background;
    struct ast_node *next;
} ast_node;

// create empty arg_node
arg_node* create_arg_node();

// create empty ast_node
ast_node* create_ast_node();

// basic check: too many special characters in a token
int basic_check(token_list_node *head);

// return valid ast, else grammar error
ast_node* build_ast(token_list_node *token_head);

#endif