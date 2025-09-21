#ifndef tokeniser
#define tokeniser

#include "common.h"

typedef struct token_list_node {
    char *token;
    struct token_list_node *next;
} token_list_node;

// returns pointer to new token_list_node
token_list_node* create_token_list_node();

// checks whitespace
int is_whitespace(char c);

// checks special char except whitespace
int is_special(char c);

// checks if append symbol is present
int is_append_symbol(char c, char d);

// splits into tokens based on spaces; returns pointer to head of 
// linked list
token_list_node* tokenise_pass1(char *input_string);

// further splits tokens into more tokens based on special characters
void tokenise_pass2(token_list_node *head);

// tokenise the input string
// by spaces and group special characters
token_list_node* tokenise(char *input_string);

#endif