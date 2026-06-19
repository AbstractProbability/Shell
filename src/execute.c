#include "../include/cd.h"
#include "../include/common.h"
#include "../include/execute.h"
#include "../include/jobs.h"
#include "../include/log.h"
#include "../include/parser.h"
#include "../include/reveal.h"

static void handle_redirect(int *p_fd_in, int *p_fd_out, ast_node *head) {
    int fd_in = *p_fd_in, fd_out = *p_fd_out;
    if (head->input_filename != NULL) {
        fd_in = open(head->input_filename, O_RDONLY);
        if (fd_in < 0) {
            fprintf(stderr, "No such file or directory!\n");
            exit(0);
        }
        dup2(fd_in, STDIN_FILENO);
    }
    if (head->output_filename != NULL) {
        if (head->output_file_mode == 1)
            fd_out = open(head->output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        else
            fd_out = open(head->output_filename, O_WRONLY | O_APPEND | O_CREAT, 0777);
        if (fd_out < 0) {
            fprintf(stderr, "Unable to create file for writing\n");
            exit(0);
        }
        dup2(fd_out, STDOUT_FILENO);
    }
}

static void handle_execution(ast_node *head, char **args_list) {
    char *command = malloc(sizeof(char) * (MAX_CHARS+1));
    char path1[] = "/bin/";
    char path2[] = "/usr/bin/";

    // try exec normally
    strcpy(command, head->command);
    args_list[0] = command;
    execv(command, args_list);

    // exec using path1
    strcpy(command, path1);
    strcat(command, head->command);
    args_list[0] = command;
    execv(command, args_list);

    // exec using path2
    strcpy(command, path2);
    strcat(command, head->command);
    args_list[0] = command;
    execv(command, args_list);

    // error if exec failed
    fprintf(stderr, "Command not found!\n");
    exit(0);
}

static void handle_close(int fd_in, int fd_out) {
    close(fd_in);
    close(fd_out);
}

// execute a single ast_node
static int execute_one(ast_node *head) {
    int fd_in = -1, fd_out = -1;
    handle_redirect(&fd_in, &fd_out, head);

    if (strcmp(head->command, "cd") == 0) {
        callcd(head->command_args_head, &parent_dir, &curr_dir, &prev_dir);
        handle_close(fd_in, fd_out);
    }
    else if (strcmp(head->command, "fg") == 0) {
        if (head->command_args_head == NULL) {
            callfg("-1");
        } else {
            callfg(head->command_args_head->command);
        }
        handle_close(fd_in, fd_out);
    }
    else if (strcmp(head->command, "bg") == 0) {
        if (head->command_args_head == NULL) {
            callbg("-1");
        } else {
            callbg(head->command_args_head->command);
        }
        handle_close(fd_in, fd_out);
    }
    else if (strcmp(head->command, "ping") == 0) {
        int cmds = 0;
        arg_node *temp = head->command_args_head;
        while (temp != NULL) {
            temp = temp->next_arg;
            cmds++;
        }
        if (cmds != 2) {
            fprintf(stderr, "ping: usage: exactly 2 args\n");
        }
        callping(head->command_args_head->command, head->command_args_head->next_arg->command);
        handle_close(fd_in, fd_out);
    }

    else if (strcmp(head->command, "jobs") == 0) {
        if (head->command_args_head != NULL) {
            fprintf(stderr, "jobs: usage: no args\n");
            exit(0);
        }
        calljobs();
        handle_close(fd_in, fd_out);
        exit(0);
    }

    else if (strcmp(head->command, "reveal") == 0) {
        reveall(head, curr_dir, parent_dir, prev_dir);
        handle_close(fd_in, fd_out);
        exit(0);
    }

    else if (strcmp(head->command, "log") == 0) {
        if (head->command_args_head == NULL) {
            if (log_head != NULL)
            log_print(log_head);
        }
        else if (strcmp(head->command_args_head->command, "purge") == 0) {
            log_purge();
        }
        else if (strcmp(head->command_args_head->command, "execute") == 0) {
            if (head->command_args_head->next_arg != NULL) {
                log_execute(atoi(head->command_args_head->next_arg->command));
            } else {
                fprintf(stderr, "execute: error executing log execute\n");
            }
            exit(0);
        }
        else {
            fprintf(stderr, "execute: error executing log\n");
        }
        handle_close(fd_in, fd_out);
    }

    else {
        arg_node *temp = head->command_args_head, *temp2;
        temp2 = temp;
        int size = 2;
        while (temp != NULL) {
            size++;
            temp = temp->next_arg;
        }

        temp = temp2;
        char **args_list = (char**) malloc(sizeof(char*) * size);

        size = 0;
        size++;

        while(temp != NULL) {
            args_list[size] = temp->command;
            temp = temp->next_arg; size++;
        }
        args_list[size] = NULL;

        handle_execution(head, args_list);
        handle_close(fd_in, fd_out);
        exit(1);
    }

    return 0;
}

static int execute_all(ast_node *head) {
    ast_node *temp = head;

    while (temp != NULL) {
        ast_node *temp2 = temp;

        int cmds = 1;
        while (temp2 != NULL && temp2->pipe_send) {
            cmds++;
            temp2 = temp2->next;
        }

        ast_node *commands[cmds], *temp3 = temp;
        int pipes[cmds-1][2];
        for (int i = 0; i<cmds-1; i++) {
            pipe(pipes[i]);
            commands[i] = temp3;
            temp3 = temp3->next;
        }
        commands[cmds-1] = temp3;
        temp3 = temp3->next;
        int pids[cmds];
        int pgid = 0;

        for (int i = 0; i<cmds; i++) {
            if (strcmp(commands[i]->command, "cd") == 0) {
                // puts("execute all cd");
                execute_one(commands[i]);
                continue;
            }
            else if (strcmp(commands[i]->command, "fg") == 0) {
                execute_one(commands[i]);
                continue;
            } else if (strcmp(commands[i]->command, "bg") == 0) {
                execute_one(commands[i]);
                continue;
            } else if (strcmp(commands[i]->command, "ping") == 0) {
                execute_one(commands[i]);
                continue;
            } else if (strcmp(commands[i]->command, "log") == 0 && commands[i]->command_args_head != NULL &&
                        strcmp(commands[i]->command_args_head->command, "execute") != 0) {
                execute_one(commands[i]);
                continue;
            }
            pids[i] = fork();

            if (pids[i] == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);

                // input for first command is stdin, not pipe
                if (i != 0) {
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }

                // output for last command is stdout, not pipe
                if (i != cmds-1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                for (int j = 0; j<cmds-1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execute_one(commands[i]);
                exit(0);
            } else {
                if (pgid == 0) pgid = pids[i];
                setpgid(pids[i], pgid);
            }
        }
        for (int i = 0; i<cmds-1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (commands[cmds-1]->background) {
            add_job_string(current_command, pgid, RUNNING);
            printf("[%d] %d\n", background_jobs_tail->serial_num, pgid);
        } else {
            foreground_pgid = pgid;
            tcsetpgrp(STDIN_FILENO, pgid);

            int wait_count = 0;
            int status;

            while (wait_count < cmds) {
                pid_t pid = waitpid(-pgid, &status, WUNTRACED);

                if (pid > 0) {
                    if (WIFSTOPPED(status)) {
                        add_job_string(current_command, pgid, STOPPED);
                        printf("[%d] Stopped %s\n", background_jobs_tail->serial_num, background_jobs_tail->command);
                        break;
                    }
                    if (WIFEXITED(status) || WIFSIGNALED(status)) {
                        wait_count++;
                    }
                } else {
                    break;
                }
            }

            tcsetpgrp(STDIN_FILENO, getpgrp());
            foreground_pgid = 0;
        }

        temp = temp2->next;
    }
    return 0;
}

void executor(char *input) {
    token_list_node *token_list_head = tokenise(input);
    // printf("tokened!\n");
    ast_node *ast_head = build_ast(token_list_head);
    if (ast_head == NULL) {
        return;
    }
    // printf("parsed!\n");
    execute_all(ast_head);
    // printf("executed!\n");
    log_store(input);
    // printf("logged!\n");
}

