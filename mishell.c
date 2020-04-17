#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "strvec.h"
#include "shell_state.h"
#include "job.h"

ShellState shell_state;
StrVec built_in;

// Stores chunks of line separated by delim into tokens
void separate(StrVec* tokens, char* line, char* delim) {
    char* token;
    while( (token=strsep(&line, delim)) != NULL ) {
        if(strlen(token) == 0)continue; // Remove whitespace
        vec_append(tokens, token);
    }
}

// If there are N piped commands c1 | c2 | ... | cN connect them so that
// c1.in = STDIN & c1.out = c2.in
// c2.in = c1.out & c2.out = c3.in
// cn.in = c(n-1).out & cn.out = STDOUT
void process_piped(StrVec piped) {
    Job* prev = NULL;
    for(int i=0;i<piped.len;i++) {
        StrVec tokens;
        vec_init(&tokens);
        separate(&tokens, vec_get(&piped, i), " ");

        if(prev == NULL) {
            // This is the first job in the list so init and continue
            Job job;
            job_init(&job, vec_get(&tokens, 0), tokens, shell_state, STDIN_FILENO, STDOUT_FILENO);
            prev = &job;
            continue;
        }

        int pipefd[2];
        int err = pipe(pipefd);

        if(err == -1) {
            report_error();
            return;
        }

        prev->out_fd = pipefd[1];

        Job* job = malloc(sizeof(Job));
        job_init(job, vec_get(&tokens, 0), tokens, shell_state, pipefd[0], STDOUT_FILENO);

        job_execute(prev);
        //free(job);
        prev = job;
    }
    job_execute(prev);
}

// Handles a single subcommand from the input
void process_parallel(char* chunk) {
    // Stores parts of the input separated by the '|'
    StrVec piped;
    vec_init(&piped);
    separate(&piped, chunk, "|");

    if(piped.len > 1) {
        process_piped(piped);
        vec_free(&piped);
        return;
    }

    vec_free(&piped);

    // If there should be a redirect open the specified file and set the job output to it 
    char* redirect = strchr(chunk, '>');
    int out_fd = STDOUT_FILENO;

    if(redirect != NULL) {
        *redirect = '\0';
        redirect++;
        out_fd = open(redirect, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU | S_ISUID);
    }

    // Execute this subcommand
    StrVec tokens;
    vec_init(&tokens);
    separate(&tokens, chunk, " ");

    Job job;
    job_init(&job, vec_get(&tokens, 0), tokens, shell_state, STDIN_FILENO, out_fd);

    shell_execute(&shell_state, job);
    vec_free(&tokens);
}

// Handle a single line from the CLI or the batch file 
void process_line(char* line) {
    // Stores parts of the input separated with the '&'
    StrVec parallel;
    vec_init(&parallel);
    separate(&parallel, line, "&");

    for(int i=0;i<parallel.len;i++) {
        char* chunk = vec_get(&parallel, i);
        process_parallel(chunk);
    }

    vec_free(&parallel);
}

char* get_prompt() {
    char* work_dir = getcwd(NULL, 0);
    char* prompt = malloc(sizeof(char) * strlen(work_dir) + 3);
    sprintf(prompt, "~%s>  ", work_dir);
    return prompt;
}


int main(int argc, char** argv) {
    // Add the built in commands
    vec_init(&built_in);
    vec_append(&built_in, "exit");
    vec_append(&built_in, "cd");
    vec_append(&built_in, "path");

    FILE* input_file = stdin;
    bool batch = false; // If 0 the shell is used as a CLI otherwise read from a file

    if(argc != 1 && argc != 2) {
        report_error();
        exit(1);
    }

    if(argc == 2) {
        input_file = fopen(argv[1], "r");
        batch = true;
    }

    shell_init(&shell_state, batch);

    if(batch == 0) {
        char* line;
        while((line = readline(get_prompt())) != NULL) {
            if(strlen(line) > 0) {
                add_history(line);
            }
            process_line(line);
            free(line);
        }
    } else {
        char* line = NULL;
        size_t line_size = 0;
        ssize_t bytes_read = 0;
        while( (bytes_read=getline(&line, &line_size, input_file))) {
            if(bytes_read == -1) break;
            
            char* last = &line[strlen(line)-1];
            if(*last == '\n')*last = '\0'; // Remove the newline (\n) character at the end of the line
            
            process_line(line);
            
        }
    }

    fclose(input_file);
}