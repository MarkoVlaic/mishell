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

void job_init(Job* job, char* command, StrVec args, ShellState shell_state, int in_fd, int out_fd) {
    job->command = command;
    job->args = args;
    job->shell_state = shell_state;
    job->in_fd = in_fd;
    job->out_fd = out_fd;
}

void job_free(Job* job) {
    close(job->in_fd);
    close(job->out_fd);
}

void job_execute(Job* job) {
    int pid = fork();

    int main_stdout = dup(STDOUT_FILENO);
    int main_stdin = dup(STDIN_FILENO);

    if(pid == -1) {
        report_error();
        exit(2);
    } else if(pid == 0) {
        if(job->in_fd != STDIN_FILENO) {
            dup2(job->in_fd, STDIN_FILENO);
            close(job->in_fd);
        }

        if(job->out_fd != STDOUT_FILENO) {
            //int out_fd = open(job->out_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
            //int err = write(out_fd, "jup life sucks", 14);
            dup2(job->out_fd, STDOUT_FILENO);
            close(job->out_fd);
            //close(job->out_fd);
        }

        char* exec_path = NULL; 
        StrVec search_path = job->shell_state.search_path;

        for(int i=0;i<search_path.len;i++) {
            char* path = strdup(vec_get(&search_path, i));
            int len = strlen(path) + strlen(job->command) + 1;
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

        close(job->out_fd);
        close(job->in_fd);

    } else {
        int child = wait(NULL);
        if(child == -1 && job->shell_state.batch)exit(3);
        job_free(job);
        dup2(main_stdout, STDOUT_FILENO);
        dup2(main_stdin, STDIN_FILENO);
    }
}