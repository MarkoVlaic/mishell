#include <stdio.h>

#include "strvec.h"
#include "shell_state.h"

#ifndef JOB_H
#define JOB_H

typedef struct Job {
    ShellState shell_state;
    StrVec args;
    char* command;
    int in_fd;
    int out_fd;
} Job;

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, int in_fd, int out_fd);
void job_free(Job* job);
void job_execute(Job* job);

#endif