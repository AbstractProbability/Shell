#include "../include/parser.h"

// create empty arg_node
arg_node* create_arg_node() {
    arg_node *newNode = malloc(sizeof(arg_node));
    newNode->command = NULL;
    newNode->next_arg = NULL;
    return newNode;
}

// create empty ast_node
ast_node* create_ast_node() {
    ast_node *newNode = malloc(sizeof(ast_node));
    newNode->command = NULL;
    newNode->command_args_head = NULL;
    newNode->command_args_tail = NULL;
    newNode->input_filename = NULL;
    newNode->output_filename = NULL;
    newNode->output_file_mode = 0;
    newNode->male = 0;
    newNode->female = 0;
    newNode->background = 0;
    newNode->next = NULL;
    return newNode;
}

// basic check: too many special characters in a token
int basic_check(token_list_node *head) {
    token_list_node *temp = head;
    while (temp != NULL) {
        int token_size = strlen(temp->token);
        if (is_special(temp->token[0])) {
            if (temp->token[0] == '>') {
                if (token_size == 2) {
                    if (temp->token[1] != '>') {
                        fprintf(stdout, "Invalid Syntax!\n");
                        return 1;
                    }
                } else if (token_size > 2) {
                    fprintf(stdout, "Invalid Syntax!\n");
                    return 1;
                }
            } else {
                if (token_size > 1) {
                    fprintf(stdout, "Invalid Syntax!\n");
                    return 1;
                }
            }
        }
        temp = temp->next;
    }
    return 0;
}

// return valid ast, else grammar error
ast_node* build_ast(token_list_node *token_head) {
    int ret = basic_check(token_head);
    if (ret != 0) return NULL;

    ast_node *ast_head = create_ast_node();
    token_list_node *token_temp = token_head;
    ast_node *ast_temp = ast_head;
    int input_symbol = 0, output_symbol = 0;
    int pipe = 0, newcmd = 0;

    while (token_temp != NULL) {
        if (is_special(token_temp->token[0]) &&
            input_symbol == 0 && output_symbol == 0 &&
            newcmd == 0  && pipe == 0 &&
            ast_temp->command != NULL
        ) 
        {
            if (token_temp->token[0] == '>') {
                if (token_temp->token[1] == '\0') {
                    output_symbol = 1;
                } else {
                    output_symbol = 2;
                }
            } else if (token_temp->token[0] == '<') {
                input_symbol = 1;
            } else {
                if (token_temp->token[0] == '|') {
                    pipe = 1;
                    ast_temp->next = create_ast_node();
                    ast_temp->male = 1;
                    ast_temp->next->female = 1;
                    ast_temp = ast_temp->next;
                } else if (token_temp->token[0] == '&') {
                    newcmd = 1;
                    ast_temp->background = 1;
                } else {
                    newcmd = 1;
                }
            }
        } 
        else if (is_special(token_temp->token[0])) 
        {
            fprintf(stdout, "Invalid Syntax!\n");
            return NULL;
        } 
        else
        {
            if (input_symbol == 0 && output_symbol == 0) {
                if (newcmd) {
                    ast_temp->next = create_ast_node();
                    ast_temp = ast_temp->next;
                }
                
                if (ast_temp->command == NULL) {
                    ast_temp->command = token_temp->token;
                } else {
                    if (ast_temp->command_args_head == NULL) {
                        ast_temp->command_args_head = create_arg_node();
                        ast_temp->command_args_head->command = token_temp->token;
                        ast_temp->command_args_tail = ast_temp->command_args_head;
                    } else {
                        ast_temp->command_args_tail->next_arg = create_arg_node();
                        ast_temp->command_args_tail = ast_temp->command_args_tail->next_arg;
                        ast_temp->command_args_tail->command = token_temp->token;
                    }
                }
            } else {
                if (input_symbol != 0) {
                    ast_temp->input_filename = token_temp->token;
                }
                if (output_symbol != 0) {
                    ast_temp->output_filename = token_temp->token;
                    ast_temp->output_file_mode = output_symbol;
                }
            }
            input_symbol = 0; output_symbol = 0;
            newcmd = 0; pipe = 0;
        }
        token_temp = token_temp->next;
    }

    
    if (ast_temp->command == NULL) {
        if (ast_head->command == NULL) {
            return NULL;
        }
        fprintf(stdout, "Invalid Syntax!\n");
        return NULL;
    }

    return ast_head;
}
