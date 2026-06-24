#include <ctype.h>

#include "../include/parser2.h"

static char SPECIAL_CHARS[] = {'&', '|', '>', '<', ';'};
static int NUM_SPECIAL_CHARS = sizeof(SPECIAL_CHARS);

// checks special char except whitespace
int is_special(char c)
{
    for (int i = 0; i<NUM_SPECIAL_CHARS; i++) {
        if (c == SPECIAL_CHARS[i]) {
            return 1;
        }
    }
    return 0;
}

static void skip_whitespace(char **p_tok)
{
    while (**p_tok && isspace(**p_tok)) {
        (*p_tok)++;
    }
}

static void skip_till_special_or_whitespace(char **p_tok)
{
    while (**p_tok && !isspace(**p_tok) && !is_special(**p_tok)) {
        (*p_tok)++;
    }
}

static char *string_from_start_end_memloc(char *start_memloc, char *end_memloc)
{
    char *ret = malloc(sizeof(char) * (end_memloc-start_memloc+1));
    printf("toksize: %d ", end_memloc-start_memloc);
    strncpy(ret, start_memloc, end_memloc-start_memloc);
    return ret;
}

/* Expects pointer to non-Null string as input and outputs the next token
 * in the string.
 * Next token is any one special character among SPECIAL_CHARS, or a
 * string of non-special, non-whitespace characters
 * Returns a COPY of the next string.
 */
char *get_next_token(char **p_input)
{
    skip_whitespace(p_input);
    char *start = *p_input;
    if (**p_input == '\0') {
        return NULL;
    }
    if (is_special(**p_input)) {
        (*p_input)++;
    } else {
        skip_till_special_or_whitespace(p_input);
    }
    return string_from_start_end_memloc(start, *p_input);
}

// create empty arg_node
static arg_node *create_arg_node(char *arg)
{
    arg_node *new_arg_node = malloc(sizeof(arg_node));

    new_arg_node->arg       = arg;
    new_arg_node->next_arg  = NULL;

    return new_arg_node;
}

static cmd_node *create_cmd_node(void)
{
    cmd_node *new_cmd_node = malloc(sizeof(cmd_node));

    new_cmd_node->command           = NULL;
    new_cmd_node->argc              = 0;
    new_cmd_node->args              = NULL;
    new_cmd_node->input_filename    = NULL;
    new_cmd_node->output_filename   = NULL;
    new_cmd_node->has_input_file    = 0;
    new_cmd_node->has_output_file   = 0;
    new_cmd_node->pipe_in           = 0;
    new_cmd_node->pipe_out          = 0;
    new_cmd_node->next              = NULL;

    return new_cmd_node;
}

static create_cmd_group_node(void)
{
    cmd_group_node *new_cmd_group_node = malloc(sizeof(cmd_group_node));

    new_cmd_group_node->cmds_head       = NULL;
    new_cmd_group_node->cmds_tail       = NULL;
    new_cmd_group_node->is_background   = 0;
    new_cmd_group_node->next            = NULL;

    return new_cmd_group_node;
}

void print_syntax_error(void)
{
    printf("Syntax error");
}

char *valid_string(char *str)
{
    if (!str) {
        return "-";
    }
    return str;
}

void print_ast(cmd_group_node *head)
{
    while (head) {
        printf("is_background: %d", head->is_background);
        cmd_node *temp = head->cmds_head;
        while (temp) {
            printf("\tcommand: %s, input_filename: %s, output_filename: %s, has_input_file: %d, has_output_file: %d, pipe_in: %d, pipe_out: %d\n",
                    temp->command,
                    temp->input_filename,
                    temp->output_filename,
                    temp->has_input_file,
                    temp->has_output_file,
                    temp->pipe_in,
                    temp->pipe_out);
            printf("\t\targs: ");
            arg_node *args = temp->args;
            while (args) {
                printf("%s ", args->arg);
                args = args->next;
            }
            printf("\n");
            temp = temp->next;
        }
        head = head->next;
    }
}

cmd_group_node *build_ast(char *input_line)
{
    char *in = input_line;
    char *token = get_next_token(&in);
    cmd_group_node *head = create_cmd_group_node();
    cmd_group_node *curr = head;

    for (token = get_next_token(&in); token; token = get_next_token(&in)) {
        switch (*token) {
        case ';':
            if (!curr->cmds_tail || !curr->cmds_tail->command) {
                print_syntax_error();
                return NULL;
            }
            curr->next = create_cmd_group_node();
            curr = curr->next;
            break;
        case '&':
            if (!curr->cmds_tail || !curr->cmds_tail->command
                || curr->is_background)
            {
                print_syntax_error();
                return NULL;
            }
            curr->is_background = 1;
            curr->next = create_cmd_group_node();
            curr = curr->next;
            break;
        case '|':
            if (!curr->cmds_tail || !curr->cmds_tail->command) {
                print_syntax_error();
                return NULL;
            }
            curr->cmds_tail = create_cmd_node();
            curr->cmds_tail->pipe_out = 1;
            curr->cmds_tail->next->pipe_in = 1;
            curr->cmds_tail = curr->cmds_tail->next;
            break;
        case '<':
            if (!curr->cmds_head || !curr->cmds_head->command
                || curr->cmds_head->has_input_file) {
                print_syntax_error();
                return NULL;
            }
            curr->cmds_head->has_input_file = 1;
            break;
        case '>':
            if (!curr->cmds_tail || !curr->cmds_tail->command) {
                print_syntax_error();
                return NULL;
            }
            if (curr->cmds_tail->has_input_file == FILE_APPEND) {
                print_syntax_error();
                return NULL;
            }
            if (curr->cmds_tail->has_input_file == FILE_TRUNCATE
                && curr->cmds_tail->input_filename)
            {
                print_syntax_error();
                return NULL;
            }
            if (curr->cmds_tail->has_input_file == FILE_TRUNCATE) {
                curr->cmds_tail->has_input_file = FILE_APPEND;
            } else {
                curr->cmds_tail->has_input_file = FILE_TRUNCATE;
            }
            break;
        default:
            break
        }
    }
    return head;
}

