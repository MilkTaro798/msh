#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>
#include <stdbool.h>

typedef enum job_state{FOREGROUND, BACKGROUND, SUSPENDED, UNDEFINED} job_state_t;

// Represents a job in a shell.
typedef struct job {
    char *cmd_line;     // The command line for this specific job.
    job_state_t state;  // The current state for this job
    pid_t pid;          // The process id for this job
    int jid;            // The job number for this job
}job_t;

/*
* add_job: Adds a new job to the job list.
*
* jobs: An array of job_t structures representing current jobs.
* max_jobs: The maximum number of jobs allowed.
* pid: The process ID of the new job.
* state: The initial state of the new job (FOREGROUND, BACKGROUND, SUSPENDED, UNDEFINED).
* cmd_line: The command line string for the new job.
*
* Returns: true if the job is successfully added, false otherwise.
*/
bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line);

/*
* delete_job: Deletes a job from the job list.
*
* jobs: An array of job_t structures representing current jobs.
* max_jobs: The maximum number of jobs allowed.
* pid: The process ID of the job to be deleted.
*
* Returns: true if the job is successfully deleted, false otherwise.
*/
bool delete_job(job_t *jobs, int max_jobs, pid_t pid);

/*
* free_jobs: Frees up resources associated with all jobs.
*
* jobs: An array of job_t structures representing current jobs.
* max_jobs: The maximum number of jobs allowed.
*
* This function does not return a value.
*/
void free_jobs(job_t *jobs, int max_jobs);

#endif