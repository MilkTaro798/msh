#include "shell.h"
#include "job.h"
#include "history.h"
#include "signal_handlers.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>


const int MAX_LINE = 1024;
const int MAX_JOBS = 16;
const int MAX_HISTORY = 10;
pid_t fgpid = 0;
pid_t wait_pid = 0;
msh_t *shell = NULL;


msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {
    shell = (msh_t *)malloc(sizeof(msh_t));
    if (!shell) return NULL;  


    shell->maxjobs = (max_jobs == 0) ? MAX_JOBS : max_jobs;
    shell->maxline = (max_line == 0) ? MAX_LINE : max_line;
    shell->maxhistory = (max_history == 0) ? MAX_HISTORY : max_history;

    shell->jobs = (job_t *)malloc(shell->maxjobs * sizeof(job_t));
    if (!shell->jobs) {
        free(shell);
        return NULL;
    }
    shell->history = alloc_history(shell->maxhistory); 
    if (!shell->history) {
        free_history(shell->history);
        free(shell);
        return NULL;
    }
    initialize_signal_handlers();
    return shell;
}

char *parse_tok(char *line, int *job_type) {
    static char *current;

    if (line != NULL) {
        current = line;
    }

    if (current == NULL || *current == '\0') {
        *job_type = -1;
        return NULL;
    }

    char *start = current;
    
    while (*current != '\0') {
        if (*current == '&' || *current == ';') {
            *job_type = (*current == '&') ? 0 : 1;
            *current = '\0';
            current++;

            while (*current == '&' || *current == ';') {
                current++;
            }

            char *temp = current;
            while (*temp != '\0' && isspace((unsigned char)*temp)) {
                temp++;
            }

            if (*temp == '\0') {
                current = temp;
            }

            return start;
        }
        current++;
    }

    *job_type = 1;
    return start;
}

char **separate_args(char *line, int *argc, bool *is_builtin) {
    *argc = 0;
    *is_builtin = false;

    if (line == NULL || *line == '\0') return NULL;

    int capacity = 20;
    char **argv = (char **)malloc(capacity * sizeof(char *));
    if (!argv) return NULL;

    char *token = strtok(line, " ");
    while (token != NULL) {
        if (*argc >= capacity) {
            capacity *= 2;
            argv = (char **)realloc(argv, capacity * sizeof(char *));
            if (!argv) return NULL;
        }
        argv[(*argc)++] = token;
        token = strtok(NULL, " ");
    }
    argv[*argc] = NULL;

    if (*argc > 0) {
        if (strcmp(argv[0], "jobs") == 0 || 
            strcmp(argv[0], "history") == 0 ||
            (argv[0][0] == '!' && argv[0][1] != '\0') ||
            (strcmp(argv[0], "bg") == 0 && *argc > 1) || 
            (strcmp(argv[0], "fg") == 0 && *argc > 1) ||
            (strcmp(argv[0], "kill") == 0 && *argc > 2)) {
            *is_builtin = true;
        }
    }

    return argv;
}

void waitfg(pid_t pid){
    while (1)
    {
        sleep(1);
        if(wait_pid == pid){
            break;
        }
    }
    
}

int evaluate(msh_t *shell, char *line) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    if (strlen(line) > shell->maxline) {
        printf("error: reached the maximum line limit\n");
        return 0;
    }
    if (line[0] == '#') {
        return 0;
    }
    int type;
    char *job_cmd;
    pid_t pid;
    int status;
    int argc;
    bool is_builtin;
    
    job_cmd = parse_tok(line, &type);
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    while (job_cmd != NULL && type != -1) {
        if (strstr(job_cmd, "exit") != NULL) {
            return 1;
        }
        
        char **argv = separate_args(job_cmd, &argc, &is_builtin);
        if (!(argv[0][0] == '!' && argv[0][1] != '\0')){
            add_line_history(shell->history, line);
        }
        if(is_builtin){
            char *result = builtin_cmd(argv);
            break;
            
        }

        
        sigprocmask(SIG_BLOCK, &mask, &prev);
        pid = fork();
        if (pid == 0) {
            setpgid(0, 0);
            execve(argv[0], argv, NULL);
            exit(EXIT_FAILURE);
        }
        else if(pid > 0){
            
            if (!add_job(shell->jobs, shell->maxjobs, pid, (type == 0) ? BACKGROUND : FOREGROUND, job_cmd)){
                printf("error: reached the maximum jobs limit\n");
            }
            sigprocmask(SIG_SETMASK, &prev, NULL);
            if (type == 1) {
                fgpid = pid;
                waitfg(pid);
                
            }
        }
        else{
            printf("error: fork failed\n");
        }
        free(argv);
        job_cmd = parse_tok(NULL, &type);
    }
    
    return 0; 
}

void exit_shell(msh_t *shell) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, 0)) != -1) {
    }
    free(shell);  
}

const char* getStateName(job_state_t state) {
    switch (state) {
        case FOREGROUND: return "FOREGROUND";
        case BACKGROUND: return "BACKGROUND";
        case SUSPENDED: return "SUSPENDED";
        case UNDEFINED: return "UNDEFINED";
    }
}

char *builtin_cmd(char **argv) {
    if (argv[0] == NULL) {
        // Empty command
        return NULL;
    }
    if (!strcmp(argv[0], "jobs")) {
        for(int i = 0; i < shell->maxhistory; i++){
            if (shell->jobs[i].jid > 0){
                printf("[%d] %d %s %s\n", shell->jobs[i].jid, shell->jobs[i].pid, getStateName(shell->jobs[i].state), shell->jobs[i].cmd_line);
            }
        }
        return NULL; 
    }
    if (!strcmp(argv[0], "history")) {
        print_history(shell->history);
        return NULL;
    }
    if (argv[0][0] == '!') {
        evaluate(shell, find_line_history(shell->history, (argv[0][1]-48)));
        return "done";
    }
    if (!strcmp(argv[0], "bg")) {
        int jid = 0;
        pid_t pid;
        if(argv[1][0] == '%'){   
            jid = (argv[1][0] - 48);
            pid = shell->jobs[jid].pid;
        }
        else{
            pid = (pid_t)atoi(argv[1]);
        }
        if (kill(pid, SIGCONT) < 0) {
            perror("Error sending SIGCONT");
        }

        return NULL;
    }
    if (!strcmp(argv[0], "fg")) {
        int jid = 0;
        pid_t pid;
        if(argv[1][0] == '%'){
            jid = atoi(argv[1] + 1);
            pid = shell->jobs[jid].pid;
        }
        else{
            pid = (pid_t)atoi(argv[1]);
        }
        if (kill(pid, SIGCONT) < 0) {
            perror("Error sending SIGCONT");
        }
        return NULL;
    }
    if (!strcmp(argv[0], "kill")) {
        if(argv[1] != "-2"&& argv[1] != "-9" && argv[1] != "-18" && argv[1] != "-19"){
            printf("error: invalid signal number\n");
            return NULL;
        }

        pid_t pid = (pid_t)atoi(argv[3]);
        if(argv[1] != "-2"){
            if (kill(pid, SIGINT) < 0) {
                perror("Error sending SIGCONT");
            }
        }
        else if(argv[1] != "-9"){
            if (kill(pid, SIGKILL) < 0) {
                perror("Error sending SIGKILL");
            }
        }
        else if(argv[1] != "-18"){
            if (kill(pid, SIGCONT) < 0) {
                perror("Error sending SIGCONT");
            }
        }
        else if(argv[1] != "-19"){
            if (kill(pid, SIGSTOP) < 0) {
                perror("Error sending SIGSTOP");
            }
        }
        return NULL;
    }
    return NULL;
}


