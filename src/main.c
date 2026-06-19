#include "../include/common.h"
#include "../include/execute.h"
#include "../include/jobs.h"    // for callexit()
#include "../include/log.h"     // for log_list_init()

jobs_list *background_jobs_head = NULL;
jobs_list *background_jobs_tail = NULL;

char hostname[MAX_CHARS+1];
char username[MAX_CHARS+1];
char *curr_dir;
char *prev_dir;
char *parent_dir; //parent directory is the one where shell is opened
char *current_command = NULL;
char *logfile;
int foreground_pgid = 0;
int shell_pgid = 0;

void prompt(void) {
    char *ac_directory = malloc(sizeof(char) * (MAX_CHARS+1));
    strcpy(ac_directory, curr_dir);
    char *temp = ac_directory;
    int i = 0, f = 1;
    while (parent_dir[i] != '\0') {
        if (ac_directory[i] == '\0' || ac_directory[i] != parent_dir[i]) {
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
    printf("<%s@%s:%s> ", username, hostname, ac_directory);
    free(temp);
}

int main(int argc, char *argv[]) {
    gethostname(hostname, MAX_CHARS);
    getlogin_r(username, MAX_CHARS);

    prev_dir = malloc(sizeof(char) * (MAX_CHARS+1));
    curr_dir = malloc(sizeof(char) * (MAX_CHARS+1));
    parent_dir = malloc(sizeof(char) * (MAX_CHARS+1));
    prev_dir[0] = '\0';
    getcwd(parent_dir, MAX_CHARS);
    getcwd(curr_dir, MAX_CHARS);
    logfile = malloc(sizeof(char) * (MAX_CHARS+1));
    strcpy(logfile, curr_dir);
    strcat(logfile, "/logfile");

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
        // printf("command: %s\n", input);
        if (x == -1) {
            if (feof(stdin)) {
                callexit();
                exit(0);
            } else {
                printf("\n");
                clearerr(stdin);
                continue;
            }
        }
        current_command = input;
        check_list();
        executor(input);

        current_command = NULL;
        free(input);
    }
    return 0;
}

