#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "strvec.h"

// Indicates if the shell is running a batch process or cli
char batch = 0;
StrVec search_path;
StrVec built_in;

void report_error() {
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message); 
}

void parse(StrVec* tokens, char* line) {
    char* token;
    while( (token=strsep(&line, " ")) != NULL ) {
        if(strlen(token) == 0)continue; // Remove remove whitespace
        vec_append(tokens, token);
    }
}

void execute_built_in(StrVec* tokens) {
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

            vec_append(&search_path, path);
        }
    }
}

void execute(StrVec* tokens) {
    if(vec_includes(&built_in, vec_get(tokens, 0))) {
        execute_built_in(tokens);
        return;
    }

    int pid = fork();

    if(pid == -1) {
        report_error();
        exit(2);
    } else if(pid == 0) {
        char* cmd = vec_get(tokens, 0);
        char* exec_path = NULL; 

        for(int i=0;i<search_path.len;i++) {
            char* path = strdup(vec_get(&search_path, i));
            printf("From search path %s\n", path);
            int len = strlen(path) + strlen(cmd);
            path = realloc(path, len * sizeof(char));
            strcat(path, cmd);
            //path[len-1] = '\0';

            printf("path %s\n", path);

            if(access(path, X_OK) == 0) {
                exec_path = path;
                break;
            }
        }

        if(exec_path == NULL) {
            report_error();
            exit(3);
        }

        vec_set(tokens, 0, exec_path);
        vec_append(tokens, NULL);

        int exec_error = execv(exec_path, tokens->elements);
        if (exec_error == -1) {
            report_error();
            if(batch == 1)exit(4);
        }

    } else {
        int child = wait(NULL);
        if(child == -1 && batch)exit(3);
    }
}

void process_line(char* line) {
    StrVec tokens;

    vec_init(&tokens);

    parse(&tokens, line);
    execute(&tokens);

    vec_free(&tokens);
}

void print_prompt(char* path) {
    printf("~%s>  ", path);
}


int main(int argc, char** argv) {
    vec_init(&search_path);
    vec_append(&search_path, "/bin/");

    vec_init(&built_in);
    vec_append(&built_in, "exit");
    vec_append(&built_in, "cd");
    vec_append(&built_in, "path");

    FILE* input_file = stdin;

    if(argc != 1 && argc != 2) {
        report_error();
        exit(1);
    }

    if(argc == 2) {
        input_file = fopen(argv[1], "r");
        batch = 1;
    }
    
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