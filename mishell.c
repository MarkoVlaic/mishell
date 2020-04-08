#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "strvec.h"
#include "shell_state.h"
#include "job.h"

// Indicates if the shell is running a batch process or cli

ShellState shell_state;
StrVec built_in;

void separate(StrVec* tokens, char* line, char* delim) {
    char* token;
    while( (token=strsep(&line, delim)) != NULL ) {
        if(strlen(token) == 0)continue; // Remove remove whitespace
        vec_append(tokens, token);
    }
}


void process_line(char* line) {
    StrVec parallel;
    vec_init(&parallel);
    separate(&parallel, line, "&");

    StrVec tokens;
    vec_init(&tokens);

    for(int i=0;i<parallel.len;i++) {
        char* cmd = vec_get(&parallel, i);
        separate(&tokens, cmd, " ");
        shell_execute(&shell_state, &tokens);
        vec_clear(&tokens);
    }

    //vec_free(&tokens);
}

void print_prompt(char* path) {
    printf("~%s>  ", path);
}


int main(int argc, char** argv) {

    vec_init(&built_in);
    vec_append(&built_in, "exit");
    vec_append(&built_in, "cd");
    vec_append(&built_in, "path");

    FILE* input_file = stdin;
    bool batch = false;

    if(argc != 1 && argc != 2) {
        report_error();
        exit(1);
    }

    if(argc == 2) {
        input_file = fopen(argv[1], "r");
        batch = true;
    }

    shell_init(&shell_state, batch);
    
    char* line = NULL;
    size_t line_size = 0;
    ssize_t bytes_read = 0;

    StrVec tokens;
    vec_init(&tokens);

    char* work_dir = getcwd(NULL, 0);

    if(batch == 0)print_prompt(work_dir);

    while( (bytes_read=getline(&line, &line_size, input_file))) {
        if(bytes_read == -1) break;
        
        char* last = &line[strlen(line)-1];
        if(*last == '\n')*last = '\0'; // Remove the newline (\n) character at the end of the line
        
        process_line(line);
        
        work_dir = getcwd(NULL, 0);
        if(batch == 0)print_prompt(work_dir);
    }

    free(line);
    fclose(input_file);
    free(work_dir);

}