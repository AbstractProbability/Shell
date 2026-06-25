#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"

static char SPECIAL_CHARS[] = {'&', '|', '>', '<', ';'};
static int NUM_SPECIAL_CHARS = sizeof(SPECIAL_CHARS);

// checks special char except whitespace
static int is_special(char c)
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
    // printf("called");
    while (**p_tok && isspace(**p_tok)) {
        // printf("!%c", *p_tok);
        (*p_tok)++;
    }
    // printf("...1...\n");
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
    strncpy(ret, start_memloc, end_memloc-start_memloc);
#ifdef DEBUG
    printf("\"%s\"(%d), ", ret, end_memloc-start_memloc);
#endif
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
    new_arg_node->next  = NULL;

    return new_arg_node;
}

static cmd_node *create_cmd_node(void)
{
    cmd_node *new_cmd_node = malloc(sizeof(cmd_node));

    new_cmd_node->command   = NULL;
    new_cmd_node->args_head = NULL;
    new_cmd_node->args_tail = NULL;
    new_cmd_node->argc      = 0;
    new_cmd_node->pipe_in   = 0;
    new_cmd_node->pipe_out  = 0;
    new_cmd_node->next      = NULL;

    return new_cmd_node;
}

static cmd_group_node *create_cmd_group_node(void)
{
    cmd_group_node *new_cmd_group_node = malloc(sizeof(cmd_group_node));

    new_cmd_group_node->cmds_head       = NULL;
    new_cmd_group_node->cmds_tail       = NULL;
    new_cmd_group_node->input_filename  = NULL;
    new_cmd_group_node->output_filename = NULL;
    new_cmd_group_node->has_input_file  = 0;
    new_cmd_group_node->has_output_file = 0;
    new_cmd_group_node->is_background   = 0;
    new_cmd_group_node->next            = NULL;

    return new_cmd_group_node;
}

static void print_syntax_error(void)
{
    printf("Syntax error\n");
}

cmd_group_node *build_ast(char *input_line)
{
    char *in = input_line;
    char *token = get_next_token(&in);
    if (!token) {
        // special case: empty string/only whitespace returns NULL
        return NULL;
    }
    cmd_group_node *head = create_cmd_group_node();
    cmd_group_node *curr = head;

    for (; token; token = get_next_token(&in)) {
        // printf("token: %s ", token);
        // continue;
        switch (*token) {
        case ';':
            if (!curr->cmds_tail || !curr->cmds_tail->command) {
                goto error;
            }

            // issue: if command just ends with ';' then last ast node
            // is not executable and empty but still there
            curr->next = create_cmd_group_node();
            curr = curr->next;
            break;
        case '&':
            if (!curr->cmds_tail || !curr->cmds_tail->command
                || curr->is_background)
            {
                goto error;
            }

            // issue: if command just ends with '&' then last ast node
            // is not executable and empty but still there
            curr->is_background = 1;
            curr->next = create_cmd_group_node();
            curr = curr->next;
            break;
        case '<':
            if (curr->has_input_file) {
                goto error;
            }
            if (curr->has_output_file && !curr->output_filename) {
                goto error;
            }

            curr->has_input_file = 1;
            break;
        case '>':
            if (curr->has_output_file == FILE_APPEND) {
                goto error;
            }
            if (curr->has_output_file == FILE_TRUNCATE
                && curr->output_filename)
            {
                goto error;
            }
            if (curr->has_input_file && !curr->input_filename) {
                goto error;
            }

            if (curr->has_output_file == FILE_TRUNCATE) {
                curr->has_output_file = FILE_APPEND;
            } else {
                curr->has_output_file = FILE_TRUNCATE;
            }
            break;
        case '|':
            if (!curr->cmds_tail || !curr->cmds_tail->command) {
                goto error;
            }

            curr->cmds_tail->next = create_cmd_node();
            curr->cmds_tail->pipe_out = 1;
            curr->cmds_tail->next->pipe_in = 1;
            curr->cmds_tail = curr->cmds_tail->next;
            break;
        default: // non-special token
            // token is part of cmd_group
            if (curr->has_input_file && !curr->input_filename) {
                curr->input_filename = token;
                continue;
            }
            if (curr->has_output_file && !curr->output_filename) {
                curr->output_filename = token;
                continue;
            }

            // token is part of cmd
            if (!curr->cmds_tail) {
                curr->cmds_tail = create_cmd_node();
                curr->cmds_head = curr->cmds_tail;
            }
            cmd_node *currcmd = curr->cmds_tail;

            // token is command name
            if (!currcmd->command) {
                currcmd->command = token;
                continue;
            }
            // token is an arg
            if (!currcmd->args_tail) {
                currcmd->args_tail = create_arg_node(token);
                currcmd->args_head = currcmd->args_tail;
                currcmd->argc++;
                continue;
            }
            currcmd->args_tail->next = create_arg_node(token);
            currcmd->args_tail = currcmd->args_tail->next;
            currcmd->argc++;
            break;  // break from switch not loop
        }
    }

    // final implicit semicolon insertion

    // 1.   There was a final explicit ';'/'&'. So there is a
    //      dangling 'curr' at the end. Just delete curr and return head.
    if (!curr->cmds_head) {
        curr = head;
        while (curr->next && curr->next->next) {
            curr = curr->next;
        }
        if (curr->next) {
            free(curr->next);
            curr->next = NULL;
        }
    }

    // 2.   Insert implicit semicolon. Same error case as the explicit
    //      semicolon check inside the switch statement in the main loop.
    //      Only the error case is considered because we dont need a new
    //      cmd_group_node because there are no more command groups
    if (!curr->cmds_tail || !curr->cmds_tail->command) {
        goto error;
    }
    return head;

error:
    print_syntax_error();
    return NULL;
}

