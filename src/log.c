#include "../include/common.h"
#include "../include/execute.h"
#include "../include/log.h"
#include "../include/parser.h"

log_node *log_head = NULL;
int curr_logs = 0;

log_node* create_log_node() {
    log_node *newnode = malloc(sizeof(log_node));
    newnode->string = NULL;
    newnode->next = NULL;
    return newnode;
}

void logfile_write() {
    log_node *temp = log_head;
    FILE *f = fopen(logfile, "w+");
    while (temp != NULL) {
        fwrite(temp->string, sizeof(char), strlen(temp->string), f);
        //fwrite("\n", sizeof(char), 1, f);
        temp = temp->next;
    }
    fclose(f);
}

void log_store2(char *input) {
    log_node *temp;

    if (log_head == NULL) {
        log_head = create_log_node();
        log_head->string = (char*) malloc(sizeof(char) * (MAX_CHARS+1));
        strcpy(log_head->string, input);
        curr_logs++;
        return;
    }

    if (strcmp(log_head->string, input) == 0) {
        return;
    }

    temp = create_log_node();
    temp->string = (char *) malloc(sizeof(char) * (MAX_CHARS+1));
    strcpy(temp->string, input);
    temp->next = log_head;
    log_head = temp;
    curr_logs++;

    if (curr_logs > 15) {
        while (temp->next->next != NULL)
            temp = temp->next;

        free(temp->next);
        temp->next = NULL;
        curr_logs = 15;
    }
    return;
}

void log_list_init() {
    if (access(logfile, F_OK) == 0) {
        FILE *f = fopen(logfile, "r");
        char *str = NULL;
        size_t input_length = 0;
        while (getline(&str, &input_length, f) >= 0) {
            log_store2(str);
        }
        if (log_head != NULL) {
            log_node *prev = log_head;
            log_node *temp = log_head->next;
            log_head->next = NULL;
            while (temp != NULL) {
                log_node *temp2 = temp->next;
                temp->next = prev;
                prev = temp;
                temp = temp2;
            }
            log_head = prev;
        }
    }
}

// log store (called from main)
void log_store(char *input) {
    log_node *temp;

    ast_node *ast_temp = build_ast(tokenise(input));
    while (ast_temp != NULL) {
        if (strcmp(ast_temp->command, "log") == 0) return;
        ast_temp = ast_temp->next;
    }

    if (log_head == NULL) {
        log_head = create_log_node();
        log_head->string = (char*) malloc(sizeof(char) * (MAX_CHARS+1));
        strcpy(log_head->string, input);
        curr_logs++;
        logfile_write();
        return;
    }

    if (strcmp(log_head->string, input) == 0) {
        return;
    }

    temp = create_log_node();
    temp->string = (char *) malloc(sizeof(char) * (MAX_CHARS+1));
    strcpy(temp->string, input);
    temp->next = log_head;
    log_head = temp;
    curr_logs++;

    if (curr_logs > 15) {
        while (temp->next->next != NULL) {
            temp = temp->next;
        }
        free(temp->next);
        temp->next = NULL;
        curr_logs = 15;
    }
    logfile_write();
    return;
}

// log print
void log_print(log_node *curr) {
    if (curr == NULL) return;

    log_print(curr->next);
    if (curr->string != NULL)
    printf("%s", curr->string);
}

// log purge
void log_purge(void) {
    log_node *temp = log_head;
    while (temp != NULL) {
        log_head = temp->next;
        free(temp);
        temp = log_head;
    }
    log_head = NULL;
    curr_logs = 0;
    logfile_write();
    return;
}

// execute logs
void log_execute(int index) {
    if (1 <= index && index <= curr_logs) {
        log_node *temp = log_head;
        int idx = 1;
        while (temp != NULL) {
            if (index == idx) {
                executor(temp->string);
                break;
            }
            idx++;
            temp = temp->next;
        }
    }
    return;
}

