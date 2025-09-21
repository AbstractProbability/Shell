#include "../include/tokeniser.h"

// checks whitespace
int is_whitespace(char c) {
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v') return 1;
    return 0;
}

// checks special char except whitespace
int is_special(char c) {
    if (c == '&' || c == '|' || c == '>' || c == '<' || c == ';') return 1;
    return 0;
}

// checks if append symbol is present
int is_append_symbol(char c, char d) {
    if (c == '>' || c == '<') return 1;
    return 0;
}

// returns a pointer to a token_list_node
token_list_node* create_token_list_node () {
    token_list_node *newTok = malloc(sizeof(token_list_node));
    newTok->token = NULL;
    newTok->next = NULL;
    return newTok;
}

// splits into tokens based on spaces; returns pointer to head of 
// linked list
token_list_node* tokenise_pass1(char *input_string) {
    int i = 0, base = 0;
    token_list_node *head = create_token_list_node(), *temp;
    temp = head;
    while (input_string[i] != '\0') {
        base = i;
        while (input_string[base] != '\0' && is_whitespace(input_string[base])) 
        {
            i++; base++;
        }
        if (input_string[base] == '\0') {
            break;
        }
        temp->next = create_token_list_node();
        temp = temp->next;
        while (input_string[i] != '\0' && !is_whitespace(input_string[i]))
        {
            i++;
        }
        
        int string_length = i-base;
        char *tok = malloc(sizeof(char) * (string_length+1));
        for (int idx = base; idx<i; idx++) {
            tok[idx-base] = input_string[idx];
        }
        tok[i-base] = '\0';
        temp->token = tok;
    }
    temp = head;
    head = head->next;
    free(temp);
    return head;
}

// further splits tokens into more tokens based on special characters
void tokenise_pass2(token_list_node *head) {
    token_list_node *temp = head;

    while (temp != NULL) {
        char *tok = temp->token;

        int i = 0, base = 0;
        token_list_node *lhead = create_token_list_node(), *ltemp;
        ltemp = lhead;
        
        while (tok[i] != '\0') {
            int spec_set = 0;
            spec_set = is_special(tok[i]);

            while (tok[i] != '\0' && is_special(tok[i]) == spec_set) {
                i++;
            }
            int string_length = i-base;
            if (string_length != 0) {
                ltemp->next = create_token_list_node();
                ltemp = ltemp->next;
                char *word = malloc(sizeof(char) * (string_length + 1));
                for (int idx = base; idx < i; idx++) {
                    word[idx-base] = tok[idx];
                }
                word[i-base] = '\0';
                ltemp->token = word;
                base = i;
            }
        }
        token_list_node *save = lhead;
        lhead = lhead->next;
        free(save);
        save = temp->next;
        
        //token_list_node *save = temp->next;
        if (lhead != NULL) {
            temp->token = lhead->token;
            temp->next = lhead->next;
            
            token_list_node *last = temp;
            while(last->next != NULL) {
                last = last->next;
            }
            last->next = save;

            free(tok);
        }
        temp = save;
    }
}

token_list_node* tokenise(char *input_string) {
    token_list_node *head = tokenise_pass1(input_string);
    tokenise_pass2(head);
    return head;
}