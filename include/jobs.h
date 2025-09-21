#ifndef jobs
#define jobs

#include "common.h"
// #include "parser.h"

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

// create an empty job
// jobs_list *create_job(ast_node *head, int pid, int status);

// create an empty job, with job arg as a string
jobs_list *create_job_string(char *string, int pid, int status);

// remove a job from backround list
void remove_job(jobs_list *temp);

// add a job to the background list
// void add_job(ast_node *head, int pid, int status);

// add a job passed as a string to the bglist
void add_job_string(char *job, int pid, int status);

// check if a job doesnt exist, or exists and is
// exited- normally: 0, abnormally: -1
// running : 1
// stopped : 2
int check_job(jobs_list *temp);

// remove any completed jobs from jobs list
void check_list();

// list all backgound jobs
void activitiess();

// fg
void fgg(int job_num);

// bg
void bgg(int job_num);


#endif