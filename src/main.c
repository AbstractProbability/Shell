#include "../include/common.h"
#include "../include/tokeniser.h"
#include "../include/parser.h"
#include "../include/hop.h"
#include "../include/reveal.h"
#include "../include/execute.h"
#include "../include/jobs.h"

jobs_list *background_jobs_head = NULL;
jobs_list *background_jobs_tail = NULL;

char *computer_name;
char *user_name;
char *current_directory;
char *previous_directory;
char *parent_directory; //parent directory is the one where shell is opened
char *current_command = NULL;
char *log_file;
int foreground_pgid = 0;
int shell_pgid = 0;

void prompt(void) {
    char *ac_directory = malloc(sizeof(char) * MAX_CHARS);
    strcpy(ac_directory, current_directory);
    char *temp = ac_directory;
    int i = 0, f = 1;
    while (parent_directory[i] != '\0') {
        if (ac_directory[i] == '\0' || ac_directory[i] != parent_directory[i]) {
            f = 0;
            break;
        }
        i++;
    }
    // parent is def substring of ac
    if (f) {
        if (ac_directory[i] == '\0' || ac_directory[i] == '/') {
            ac_directory[i-1] = '~';
            ac_directory = ac_directory + i-1;
        }
    }
    printf("<%s@%s:%s> ", user_name, computer_name, ac_directory);
    free(temp);
}

void handle_input(char *input) {
    token_list_node *token_list_head = tokenise(input);
    
    ast_node *ast_head = build_ast(token_list_head);
    if (ast_head == NULL) {
        return;
    }
    execute_all(ast_head);
    log_store(input);
    //check_list();
}

int main(int argc, char *argv[]) {
    //char *str = NULL;
    computer_name = malloc(sizeof(char) * MAX_CHARS);
    user_name = malloc(sizeof(char) * MAX_CHARS);
    gethostname(computer_name, MAX_CHARS);
    getlogin_r(user_name, MAX_CHARS);

    previous_directory = malloc(sizeof(char) * MAX_CHARS);
    current_directory = malloc(sizeof(char) * MAX_CHARS);
    parent_directory = malloc(sizeof(char) * MAX_CHARS);
    previous_directory[0] = '\0';
    getcwd(parent_directory, MAX_CHARS);
    getcwd(current_directory, MAX_CHARS);
    log_file = malloc(sizeof(char) * MAX_CHARS);
    strcpy(log_file, current_directory);
    strcat(log_file, "/log_file");

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    shell_pgid = getpid();
    setpgid(shell_pgid, shell_pgid);

    tcsetpgrp(STDIN_FILENO, shell_pgid);

    log_list_init();
    
    while(1) {
        prompt();
        char *input = NULL;
        size_t input_length = 0;
        int x = getline(&input, &input_length, stdin);
        if (x == -1) {
            if (feof(stdin)) {
                ctrld();
                exit(0);
            } else {
                printf("\n");
                clearerr(stdin);
                continue;
            }
        }
        current_command = input;
        check_list();
        handle_input(input);

        current_command = NULL;
        free(input);
    }
    return 0;
}