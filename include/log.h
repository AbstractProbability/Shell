#ifndef LOG_H
#define LOG_H

typedef struct log_node {
    char *string;
    struct log_node *next;
} log_node;

extern log_node *log_head;
// log_node *log_tail = NULL;
extern int curr_logs;

#define MAX_LOGS 15
//create a log node
log_node *create_log_node();
// init log list
void log_list_init();
// log store (called from main)
void log_store(char *input);
// log print
void log_print(log_node *curr);
// purge logs
void log_purge();
// execute logs
void log_execute(int index);

#endif

