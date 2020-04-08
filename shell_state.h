#include <stdbool.h>
#include "strvec.h"

#ifndef SHELL_STATE_H
#define SHELL_STATE_H

struct Job;

typedef struct ShellState {
    StrVec search_path;
    StrVec built_in;
    bool batch;
} ShellState;

void shell_init(ShellState* shell_state, bool batch);
void shell_execute(ShellState* shell_state, struct Job* job);
void shell_execute_built_in(ShellState* shell_state, StrVec* tokens);

// Not really tied to the struct but a global shell function
void report_error();

#endif