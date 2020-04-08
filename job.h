#include <stdio.h>

#include "strvec.h"
#include "shell_state.h"

#ifndef JOB_H
#define JOB_H

typedef struct Job {
    FILE* in;
    FILE* out;
    ShellState shell_state;
    char* command;
    StrVec args;
} Job;

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, FILE* in, FILE* out);
void job_execute(Job* job);

#endif