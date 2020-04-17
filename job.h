#include <stdio.h>

#include "strvec.h"
#include "shell_state.h"

#ifndef JOB_H
#define JOB_H

// Describes a runnable instance of a command
typedef struct Job {
    ShellState shell_state; // The context in which the command is executed
    StrVec args; // Arguments to the command
    char* command; // Command name
    // These are used for output redirection and pipes
    int in_fd; // File descriptor of the input file
    int out_fd; // File descriptor of the output file
} Job;

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, int in_fd, int out_fd);
void job_free(Job* job);
// Execute this job as a child proccess of the shell 
void job_execute(Job* job);

#endif