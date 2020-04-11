#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#include "job.h"
#include "strvec.h"
#include "shell_state.h"

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, char* in_path, char* out_path) {
    job->command = command;
    job->args = args;
    job->shell_state = shell_state;
    job->in_path = in_path;
    job->out_path = out_path;
}

void job_execute(Job* job) {
    int pid = fork();

    if(pid == -1) {
        report_error();
        exit(2);
    } else if(pid == 0) {
        if(job->out_path != NULL) {
            int out_fd = open(job->out_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
            //int err = write(out_fd, "jup life sucks", 14);
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
            //close(job->out_fd);
        }

        char* exec_path = NULL; 
        StrVec search_path = job->shell_state.search_path;

        for(int i=0;i<search_path.len;i++) {
            char* path = strdup(vec_get(&search_path, i));
            int len = strlen(path) + strlen(job->command);
            path = realloc(path, len * sizeof(char));
            strcat(path, job->command);
            //path[len-1] = '\0';

            if(access(path, X_OK) == 0) {
                exec_path = path;
                break;
            }
        }

        if(exec_path == NULL) {
            report_error();
            exit(3);
        }

        vec_set(&job->args, 0, exec_path);
        vec_append(&job->args, NULL);

        int exec_error = execv(exec_path, job->args.elements);
        if (exec_error == -1) {
            report_error();
            if(job->shell_state.batch)exit(4);
        }

    } else {
        int child = wait(NULL);
        if(child == -1 && job->shell_state.batch)exit(3);
    }
}