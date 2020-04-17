#include <stdbool.h>
#include "strvec.h"

#ifndef SHELL_STATE_H
#define SHELL_STATE_H

struct Job;

// Holds global shell state and provides some core functions 
typedef struct ShellState {
    StrVec search_path; // Where to search for the commands
    StrVec built_in; // List of built in commands
    bool batch;
} ShellState;

void shell_init(ShellState* shell_state, bool batch);
// Determine if the job is a built in or custom command and run it accordingly
void shell_execute(ShellState* shell_state, struct Job job);
// Execute a built in command described by tokens
void shell_execute_built_in(ShellState* shell_state, StrVec* tokens);

// Not really tied to the struct but a global shell function
void report_error();

#endif