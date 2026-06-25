#include <stdio.h>
#include <stdlib.h>

#include "../include/parser2.h"

char *valid_string(char *str)
{
    if (!str) {
        return "-";
    }
    return str;
}

void print_ast(cmd_group_node *cmd_group)
{
    printf("\n");
    while (cmd_group) {
        printf("has_input_file: %d, input_filename: %s\n",
                cmd_group->has_input_file, valid_string(cmd_group->input_filename));
        printf("has_output_file: %d, output_filename: %s\n",
                cmd_group->has_output_file, valid_string(cmd_group->output_filename));
        printf("is_background: %d\n", cmd_group->is_background);

        cmd_node *cmd = cmd_group->cmds_head;
        while (cmd) {
            printf("\tcommand: %s, pipe_in: %d, pipe_out: %d\n",
                    valid_string(cmd->command),
                    cmd->pipe_in,
                    cmd->pipe_out);
            printf("\t\targs: ");
            arg_node *args = cmd->args_head;
            while (args) {
                printf("%s ", valid_string(args->arg));
                args = args->next;
            }
            printf("\n");
            cmd = cmd->next;
        }
        cmd_group = cmd_group->next;
    }
    printf("\n");
}

void printer(char *input_string)
{
    print_ast(build_ast(input_string));
    printf("no error\n");
}


int main() {
    char *str[] = {
        /* valid */
        " cmd "
        ,"abracadabra alakazam"
        ,"cmd0 cmd1 cmd2 cmd3 cmd4"
        ,">inp <out lol lmao --xd -cfg"
        ,"lol | gig | tee & new | pipeline"
        ,"lol | gig <inp | tee >out"
        ,"cmd arg1| cmd1 | <inp >out lol;"
        ,""     // does not work, gives syntax error
        ," "    // does not work, gives syntax error
        //
        // /* invalid */
        ,";"
        ,";;"
        ,"&;"
        ,"cmd &;"
        ,"cmd > <inp out;"
        ,"cmd | cmd1 |;"
        ,"cmd | cmd1 |;"
        ,"cmd | cmd1 | <inp >out" // dangling node passes
        ," >;"

    };
    printf("%d\n", sizeof(str));
    for (int i = 0; i<sizeof(str)/sizeof(char *); i++) {
        printer(str[i]);
    }
    return 0;
}

