#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell_state.h"
#include "job.h"

void shell_init(ShellState* shell_state, bool batch) {
    StrVec search_path;
    vec_init(&search_path);
    vec_append(&search_path, "/bin/");

    shell_state->search_path = search_path;

    StrVec built_in;
    vec_init(&built_in);
    vec_append(&built_in, "exit");
    vec_append(&built_in, "cd");
    vec_append(&built_in, "path");

    shell_state->built_in = built_in;

    shell_state->batch = batch;

}

void execute_built_in(ShellState* shell_state, StrVec* tokens) {
    char* cmd = vec_get(tokens, 0);

    if(strcmp(cmd, "exit") == 0) {
        exit(0);
    } else if(strcmp(cmd, "cd") == 0) {
        if(tokens->len != 2) {
            report_error();
            return;
        }

        chdir(vec_get(tokens, 1));
    } else if(strcmp(cmd, "path") == 0) {
        for(int i=1;i<tokens->len;i++) {
            char* path = strdup(vec_get(tokens, i));
            int len = strlen(path);
            
            if(path[len-1] != '/') {
                path = (char*) realloc(path, (len+2) * sizeof(char));
                path[len] = '/';
            }

            vec_append(&shell_state->search_path, path);
        }
    }
}

void shell_execute(ShellState* shell_state, Job job) {
    if(vec_includes(&shell_state->built_in, vec_get(&job.args, 0))) {
        execute_built_in(shell_state, &job.args);
        return;
    }
    
    job_execute(&job);
    
}

void report_error() {
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message); 
}
