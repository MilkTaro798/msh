#include "signal_handlers.h"
#include "shell.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h>



/*
* sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
*     a child job terminates (becomes a zombie), or stops because it
*     received a SIGSTOP or SIGTSTP signal. The handler reaps all
*     available zombie children, but doesn't wait for any other
*     currently running children to terminate.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigchld_handler(int sig)
{
    int saved_errno = errno;
    wait_pid = waitpid(-1, NULL, WNOHANG);
    delete_job(shell->jobs, shell->maxjobs, wait_pid);
    errno = saved_errno;
}


/*
* sigint_handler - The kernel sends a SIGINT to the shell whenver the
*    user types ctrl-c at the keyboard.  Catch it and send it along
*    to the foreground job.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigint_handler(int sig)
{
    int errno_saved = errno;
    if (fgpid != 0) {
        kill(-fgpid, SIGINT);
    }
    errno = errno_saved;

}

/*
* sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
*     the user types ctrl-z at the keyboard. Catch it and suspend the
*     foreground job by sending it a SIGTSTP.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigtstp_handler(int sig)
{
    int errno_saved = errno;
    if (fgpid != 0) {
        kill(-fgpid, SIGTSTP);
        for (int i = 0; i < shell->maxjobs; i++) {
            if(shell->jobs[i].pid == fgpid){
                shell->jobs[i].state = SUSPENDED;
            }
        }
    }
    errno = errno_saved;

}

/*
* setup_handler - wrapper for the sigaction function
*
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
typedef void handler_t(int);
handler_t *setup_handler(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0) {
        perror("Signal error");
        exit(1);
    }
    return (old_action.sa_handler);
}


void initialize_signal_handlers() {

    // sigint handler: Catches SIGINT (ctrl-c) signals.
    setup_handler(SIGINT,  sigint_handler);   /* ctrl-c */
    // sigtstp handler: Catches SIGTSTP (ctrl-z) signals.
    setup_handler(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    // sigchld handler: Catches SIGCHILD signals.
    setup_handler(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
}
