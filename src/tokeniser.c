#include "../include/tokeniser.h"

static char SPECIAL_CHARS[] = {'&', '|', '>', '<', ';'};
static int NUM_SPECIAL_CHARS = sizeof(SPECIAL_CHARS);

// checks special char except whitespace
int is_special(char c) {
    for (int i = 0; i<NUM_SPECIAL_CHARS; i++) {
        if (c == SPECIAL_CHARS[i]) {
            return 1;
        }
    }
    return 0;
}

// returns a pointer to a token_list_node
static token_list_node *get_token_list_node(char *token) {
    token_list_node *new_tok = malloc(sizeof(token_list_node));
    new_tok->token = token;
    new_tok->next = NULL;
    return new_tok;
}

static void skip_whitespace(char **p_tok_start) {
    while (**p_tok_start && isspace(**p_tok_start)) {
        (*p_tok_start)++;
    }
}

static void skip_nonwhitespace(char **p_tok_end) {
    while (**p_tok_end && !isspace(**p_tok_end)) {
        (*p_tok_end)++;
    }
}

// splits into tokens based on whitespaces
// returns pointer to head of linked list
static token_list_node *tokenise_whitespace(char *input_string) {
    token_list_node *head = get_token_list_node(NULL), *temp = head;
    char *tok_start = input_string;

    while (*tok_start) {
        skip_whitespace(&tok_start);
        if (!(*tok_start)) {
            break;
        }
        char *tok_end = tok_start;
        skip_nonwhitespace(&tok_end);

        temp->next = get_token_list_node(strndup(tok_start, tok_end-tok_start));

        temp = temp->next;
        tok_start = tok_end;
    }

    temp = head;
    head = head->next;
    free(temp);
    return head;
}

static void skip_till_dissimilar(char *tok_start, char **p_tok_end) {
    while (**p_tok_end && is_special(**p_tok_end) == is_special(*tok_start)) {
        (*p_tok_end)++;
    }
}

token_list_node *repair_list(
        token_list_node **p_ihead,
        token_list_node **p_itemp,
        token_list_node **p_temp)
{
    token_list_node *save = *p_ihead;
    *p_ihead = (*p_ihead)->next;
    free(save);

    (*p_itemp)->next = (*p_temp)->next;
    save = (*p_temp)->next;

    free((*p_temp)->token);
    (*p_temp)->token = (*p_ihead)->token;

    // ihead possibly be equal to itemp because the token contained
    // only special char or only non-special char. In this case
    // temp->next should not be set to ihead->next to keep the list intact
    if (*p_ihead != *p_itemp) {
        (*p_temp)->next = (*p_ihead)->next;
    }
    free(*p_ihead);
    return save;
}

// further splits tokens into more tokens based on special characters
static void tokenise_pass2(token_list_node *head) {
    token_list_node *temp = head;

    while (temp) {
        char *tok_start = temp->token;

        token_list_node *ihead = get_token_list_node(NULL), *itemp;
        itemp = ihead;

        // create linked list of tokens, each containing bunch of special
        // characters or a bunch of non-special characters, linked list
        // starts at ihead
        while (tok_start) {
            if (!(*tok_start)) {
                break;
            }
            char *tok_end = tok_start;
            skip_till_dissimilar(tok_start, &tok_end);

            itemp->next = get_token_list_node(strndup(tok_start, tok_end-tok_start));

            itemp = itemp->next;
            tok_start = tok_end;
        }
        temp = repair_list(&ihead, &itemp, &temp);
    }
}

void spill_tokens(token_list_node *head) {
    while (head != NULL) {
        printf("\"%s\" ", head->token);
        head = head->next;
    }
    printf("\n");
}

token_list_node* tokenise(char *input_string) {
    token_list_node *head = tokenise_whitespace(input_string);
    tokenise_pass2(head);
    // spill_tokens(head);
    return head;
}

