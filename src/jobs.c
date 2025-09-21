#include "../include/jobs.h"

jobs_list *create_job_string(char *string, int pid, int status) {
    jobs_list *newnode = (jobs_list *) malloc(sizeof(jobs_list));
    newnode->next = NULL;
    newnode->prev = NULL;
    newnode->pid = pid;
    newnode->status = status;
    newnode->serial_num = -1;

    int len = strlen(string);
    
    char *str = (char*) malloc(sizeof(char) * (len+1));
    strcpy(str, string);
    
    if (str[len-2] == '&') str[len-2] = '\0';
    else if (str[len-1] == '\n') str[len-1] = '\0';
    else str[len] = '\0';
    newnode->command = str;

    return newnode;
}

void remove_job(jobs_list *temp) {
    if (temp == NULL) return;

    if (temp == background_jobs_head) {
        background_jobs_head = background_jobs_head->next;
        if (background_jobs_head != NULL) {
            background_jobs_head->prev = NULL;
        }
        return;
    }
    if (temp == background_jobs_tail) {
        background_jobs_tail = background_jobs_tail->prev;
        if (background_jobs_tail != NULL) {
            background_jobs_tail->next = NULL;
        }
        return;
    }

    if (temp->next != NULL) {
        temp->next->prev = temp->prev;
    }
    if (temp->prev != NULL) {
        temp->prev->next = temp->next;
    }
    
    // correct_serial_numbers();
    free(temp->command);
    free(temp);
}

void add_job_string(char *job, int pid, int status) {
    jobs_list *temp = create_job_string(job, pid, status);

    if (background_jobs_head == NULL) {
        background_jobs_head = temp;
        background_jobs_tail = temp;
        temp->serial_num = 1;
        return;
    }

    background_jobs_tail->next = temp;
    temp->prev = background_jobs_tail;
    background_jobs_tail = temp;
    temp->serial_num = temp->prev->serial_num+1;
    return;
}

int check_job(jobs_list *temp) { // return 1 if running, 2 if stopped, 0 if terminated normally, -1 if terminated abnormally
    int proc_status;
    int x = waitpid(-temp->pid, &proc_status, WNOHANG);

    if (x == 0) {
        return temp->status;
    } else if (x > 0) {
        if (WIFSTOPPED(proc_status)) return STOPPED;
        else if (WIFEXITED(proc_status)) return NORMAL_TERMINATION;
        else if (WIFSIGNALED(proc_status)) return ABNORMAL_TERMINATION;
        else return RUNNING;
    } else {
        return NORMAL_TERMINATION;
    }
}

// checks the bgjobs list if any jobs are done, and prints them as so. 
void check_list() {
    jobs_list *temp = background_jobs_head;

    while (temp != NULL) {
        int x = check_job(temp);
        if (x == NORMAL_TERMINATION) {
            printf("%s with pid %d exited normally\n", temp->command, temp->pid);
            jobs_list *p = temp;
            temp = temp->next;
            remove_job(p);
        } else if (x == ABNORMAL_TERMINATION) {
            printf("%s with pid %d exited abnormally\n", temp->command, temp->pid);
            jobs_list *p = temp;
            temp = temp->next;
            remove_job(p);
        } else {
            temp->status = x;
            temp = temp->next;
        }
    }
}

int compar(const void *a, const void *b) {
    jobs_list *ja = * ((jobs_list **)a);
    jobs_list *jb = * ((jobs_list **)b);
    return strcmp(ja->command, jb->command);
}

void activitiess() {
    // check_list();
    // correct_serial_numbers();
    if (background_jobs_head == NULL) {
        printf("No bg jobs\n");
        return;
    }
    int n_jobs = 0;
    jobs_list *temp = background_jobs_head;
    while (temp != NULL) {
        n_jobs++; temp = temp->next;
    }

    jobs_list *a[n_jobs];
    temp = background_jobs_head;
    int i = 0;
    while (i < n_jobs) {
        a[i] = temp;
        i++; temp = temp->next;
    }

    qsort(a, i, sizeof(jobs_list *), compar);
    for (int j = 0; j<i; j++) {
        char status[15];
        if (a[j]->status == STOPPED) strcpy(status, "Stopped");
        else strcpy(status, "Running");
        printf("[%d] : %s - %s\n", a[j]->pid, a[j]->command, status);
    }
    exit(0);
}

void grant_foreground(jobs_list *temp) {
    printf("%s\n", temp->command);
    kill(-temp->pid, SIGCONT);
    tcsetpgrp(STDIN_FILENO, temp->pid);
    int st;
    int x = waitpid(-temp->pid, &st, WUNTRACED);
    if (x > 0) {
        if (WIFSTOPPED(st)) {
            temp->status = STOPPED;
            printf("[%d] Stopped %s\n", temp->serial_num, temp->command);
        }
    }
    tcsetpgrp(STDIN_FILENO, getpgrp());
}

void fgg(int job_num) {
    jobs_list *temp;
    if (job_num == -1) {
        temp = background_jobs_tail;
        if (temp == NULL) {
            printf("No such job\n");
            return;
        }
        while (temp != NULL && (check_job(temp) == NORMAL_TERMINATION || check_job(temp) == ABNORMAL_TERMINATION)) 
        {
            temp = temp->prev;
        }

        if (temp != NULL) {
            grant_foreground(temp);
        } else {
            printf("Job already running\n");
        }
    } else {
        temp = background_jobs_head;
        while (temp != NULL && temp->serial_num != job_num) temp = temp->next;
        if (temp != NULL && check_job(temp) != NORMAL_TERMINATION && check_job(temp) != ABNORMAL_TERMINATION) {
            grant_foreground(temp);
        } else {
            printf("No such job\n");
        }
    }
}

void bgg(int job_num) {
    if (job_num == -1) {
        jobs_list *temp = background_jobs_tail;
        while (temp != NULL && (check_job(temp) != STOPPED)) {
            temp = temp->prev;
        }
        if (temp != NULL) {
            kill(-temp->pid, SIGCONT);
            temp->status = RUNNING;
            printf("[%d] %s &\n", temp->serial_num, temp->command);
        } else {
            printf("No such job\n");
        }
    } else {
        jobs_list *temp = background_jobs_head;
        while (temp != NULL && temp->serial_num != job_num) {
            temp = temp->next;
        }
        if (temp != NULL) {
            if (check_job(temp) == STOPPED) {
                kill(-temp->pid, SIGCONT);
                temp->status = RUNNING;
                printf("[%d] %s &\n", temp->serial_num, temp->command);
            } else {
                printf("Job already running\n");
            }
        } else {
            printf("No such job\n");
        }
    }
}