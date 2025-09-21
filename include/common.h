#ifndef common
#define common

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

#define MAX_CHARS 5000
#define RUNNING 1
#define STOPPED 2
#define NORMAL_TERMINATION 0
#define ABNORMAL_TERMINATION -1
#define NO_STATUS -2

extern char *computer_name;
extern char *user_name;
extern char *current_directory;
extern char *previous_directory;
extern char *parent_directory; //parent directory is the one where shell is opened
extern char *current_command;
extern char *log_file;
extern int foreground_pgid;
extern int shell_pgid;

// ping
void pingg(char *pid_str, char *signum_str);

// EOF, end shell and kill all child
void ctrld();

#endif