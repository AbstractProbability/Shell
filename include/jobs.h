#ifndef JOBS_H
#define JOBS_H

typedef struct jobs_list {
    int serial_num;
    int pid;
    int status; // -1: stopped, 0: running, 1: running, 2: stopped
    char *command;
    struct jobs_list *next;
    struct jobs_list *prev;
} jobs_list;

// list is in reverse order of starting time
extern jobs_list *background_jobs_head;
extern jobs_list *background_jobs_tail;

// add a job passed as a string to the bglist
void add_job_string(char *job, int pid, int status);

// remove any completed jobs from jobs list
void clean_list(void);

// list all backgound jobs
void calljobs(void);

// fg
void callfg(char *job_num_string);

// bg
void callbg(char *job_num_string);

// ping
void callping(char *pid_str, char *signum_str);

// exit: EOF, end shell and kill all child
void callexit(void);

#endif

