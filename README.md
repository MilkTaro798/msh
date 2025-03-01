# MSH (MPCS Shell)

MSH is a Unix-like shell developed as part of the MPCS course. It implements various features of a typical shell, including job control, signal handling, history expansion, and built-in commands.

## Features

- Support for running multiple jobs (foreground and background)
- Signal handling for SIGINT (Ctrl-C), SIGTSTP (Ctrl-Z), and SIGCHLD
- Command history with history expansion using the `!N` syntax
- Built-in commands: `jobs`, `history`, `bg`, `fg`, and `kill`
- Redirection of standard input and standard output to files

## Repository Structure

The repository is structured as follows:

```
├── bin
├── data
├── include
│   ├── history.h
│   ├── job.h
│   ├── shell.h
│   └── signal_handlers.h
├── README.md
├── Makefile
├── scripts
│   └── build.sh
├── src
│   ├── history.c
│   ├── job.c
│   ├── msh.c
│   ├── shell.c
│   └── signal_handlers.c
└── tests
```

- `bin`: Contains the `msh` executable generated by the compiler
- `data`: Includes auxiliary data files needed by the testcases and the `.msh_history` file
- `include`: Contains header files for the modules associated with building the `msh` executable
- `scripts`: Contains the `build.sh` script for building the `msh` executable
- `src`: Includes all the source files for the modules implemented
- `tests`: Contains the testcases for testing the various modules

## Building and Running

To build the `msh` executable, navigate to the root directory of the repository and run:

```
$ gcc -I./include/ -o ./bin/msh src/*.c
```

You can then run the `msh` shell using:

```
$ ./bin/msh
```

Alternatively, you can use the provided `build.sh` script in the `scripts` directory to build the executable.

## Modules

### Shell Module (shell.h and shell.c)

The shell module contains the main logic for the shell and its state. It includes functions for parsing command lines, separating arguments, and evaluating commands.

### Job Module (job.h and job.c)

The job module handles the management of jobs within the shell. It provides functions for adding, deleting, and modifying jobs in the job array.

### History Module (history.h and history.c)

The history module implements the command history feature. It allows adding command lines to the history, printing the history, finding a specific command line from the history, and saving/loading the history to/from a file.

### Signal Handlers Module (signal_handlers.h and signal_handlers.c)

The signal handlers module sets up the necessary signal handlers for the shell. It includes handlers for SIGINT (Ctrl-C), SIGTSTP (Ctrl-Z), and SIGCHLD signals.

## Built-in Commands

MSH supports the following built-in commands:

- `jobs`: Lists all the currently active jobs
- `history`: Prints the command history
- `!N`: Reruns the Nth command from the history list
- `bg <job>`: Restarts a suspended job and runs it in the background
- `fg <job>`: Restarts a suspended job and runs it in the foreground
- `kill SIG_NUM PID`: Sends the specified signal (SIG_NUM) to the process with the given PID


## Acknowledgments

- [Computer Systems: A Programmer's Perspective, Third Edition](https://csapp.cs.cmu.edu/) by Bryant and O'Hallaron, for guidance on signal handling and job control

## Author

Yutong