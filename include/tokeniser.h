#ifndef tokeniser
#define tokeniser

#include "common.h"
#include <ctype.h>

typedef struct token_list_node {
    char *token;
    struct token_list_node *next;
} token_list_node;

// checks special char except whitespace
int is_special(char c);

// tokenise the input string
// by spaces and group special characters
token_list_node *tokenise(char *input_string);

#endif

