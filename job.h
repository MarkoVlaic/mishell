#include <stdio.h>

#include "strvec.h"
#include "shell_state.h"

#ifndef JOB_H
#define JOB_H

typedef struct Job {
    ShellState shell_state;
    StrVec args;
    char* command;
    char* in_path;
    char* out_path;
} Job;

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, char* in_path, char* out_path);
void job_execute(Job* job);

#endif