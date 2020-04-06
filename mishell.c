#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "strvec.h"

// Indicates if the shell is running a batch process or cli
char batch = 0;

void report_error() {
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message); 
}

void parse(StrVec* tokens, char* line) {
    char* token;
    while( (token=strsep(&line, " ")) != NULL ) {
        vec_append(tokens, token);
    }
}

StrVec search_path;

void execute(StrVec* tokens) {
    int pid = fork();

    if(pid == -1) {
        report_error();
        exit(2);
    } else if(pid == 0) {
        char* cmd = vec_get(tokens, 0);
        char* exec_path = NULL; 

        for(int i=0;i<search_path.len;i++) {
            char* path = strdup(vec_get(&search_path, i));
            int len = strlen(path) + strlen(cmd);
            path = realloc(path, len * sizeof(char));
            strcat(path, cmd);
            //path[len-1] = '\0';

            if(access(path, X_OK) == 0) {
                printf("gud");
                exec_path = path;
                break;
            } else {
                perror("No access");
            }
        }

        if(exec_path == NULL) {
            report_error();
            if(batch == 1) exit(3);
            return;
        }

        vec_set(tokens, 0, exec_path);
        vec_append(tokens, NULL);

        int exec_error = execv(exec_path, tokens->elements);
        if (exec_error == -1) {
            report_error();
            if(batch == 1)exit(4);
        }

    } else {
        wait(NULL);
    }
}

void process_line(char* line) {
    StrVec tokens;

    vec_init(&tokens);

    parse(&tokens, line);
    execute(&tokens);

    vec_free(&tokens);
}


int main(int argc, char** argv) {
    vec_init(&search_path);
    vec_append(&search_path, "/bin/");

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

    while( (bytes_read=getline(&line, &line_size, input_file))) {
        if(bytes_read == -1) break;
        char* last = &line[strlen(line)-1];
        if(*last == '\n')*last = '\0'; // Remove the newline (\n) character at the end of the line
        process_line(line);
    }

    free(line);
    fclose(input_file);

}