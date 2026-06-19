#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_CHARS 4096
#define RUNNING 1
#define STOPPED 2
#define NORMAL_TERMINATION 0
#define ABNORMAL_TERMINATION -1
#define NO_STATUS -2

extern char hostname[];
extern char username[];
extern char *curr_dir;
extern char *prev_dir;
extern char *parent_dir; //parent directory is the one where shell is opened
extern char *current_command;
extern char *logfile;
extern int foreground_pgid;
extern int shell_pgid;

// ping
void callping(char *pid_str, char *signum_str);

#endif

