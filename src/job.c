#include "job.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line){
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].cmd_line == NULL) {
            jobs[i].cmd_line = strdup(cmd_line);
            if (jobs[i].cmd_line == NULL) {
                return false;
            }
            jobs[i].state = state;
            jobs[i].pid = pid;
            jobs[i].jid = i + 1;  // Assuming job ID starts from 1.

            return true;  // Job added successfully.
        }
    }

    return false;
}

bool delete_job(job_t *jobs, int max_jobs, pid_t pid) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].pid == pid) {
            if (jobs[i].cmd_line != NULL) {
                free(jobs[i].cmd_line);
                jobs[i].cmd_line = NULL; 
            }

            jobs[i].state = UNDEFINED;
            jobs[i].pid = -1;  
            jobs[i].jid = -1;  

            return true;  
        }
    }

    return false;
}

void free_jobs(job_t *jobs, int max_jobs) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].cmd_line != NULL) {
            free(jobs[i].cmd_line);
            jobs[i].cmd_line = NULL; 
        }

        jobs[i].state = UNDEFINED;
        jobs[i].pid = -1;
        jobs[i].jid = -1;
    }
    free(jobs);
}