#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"

int main(int argc, char *argv[]) {
    int opt;
    int maxLine = 0, maxJobs = 0, maxHistory = 0;
    opterr = 0;

    while ((opt = getopt(argc, argv, "s:j:l:")) != -1) {
        switch (opt) {
            case 's':
                if (sscanf(optarg, "%d", &maxHistory) != 1 || maxHistory <= 0) {
                    printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 1;
                }
                break;
            case 'j':
                if (sscanf(optarg, "%d", &maxJobs) != 1 || maxJobs <= 0) {
                    printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 1;
                }
                break;
            case 'l':
                if (sscanf(optarg, "%d", &maxLine) != 1 || maxLine <= 0) {
                    printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 1;
                }
                break;
            default:
                printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                return 1;
        }
    }

    if (argv[optind] != NULL) {
        printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
        return 1;
    }

    msh_t *shell = alloc_shell(maxJobs, maxLine, maxHistory);
    if (!shell) {
        fprintf(stderr, "Failed to allocate shell\n");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nRead;

    printf("msh> ");
    while ((nRead = getline(&line, &len, stdin)) != -1) {
        int status = evaluate(shell, line);
        if (status != 0) {
            break;
        }
        printf("msh> ");
        free(line);
        line = NULL;
    }

    free(line);
    exit_shell(shell);
    return 0;
}