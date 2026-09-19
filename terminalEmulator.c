#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ARGS 64
#define MAX_LINE 1024

/* This is a simple terminal emulator implementation in C */
//Reads input from the terminal//

static int tokenize(char *line, char *argv[MAX_ARGS]) {
    int argc = 0;
    char *tok = strtok(line, " \t\r\n");
    while (tok != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t\r\n");
    }
    argv[argc] = NULL;
    return argc;
}

// Fork() to create a child process 
static int execute_command(char *argv[]){
    //get pid from the parent process
    pid_t pid = fork();

    if(pid < 0){
        perror("Error");
        return -1;
    }

    if(pid == 0){ // in the child process
        execvp(argv[0], argv); //returns only if fails 

        fprintf(stderr,"%s: %s\n", argv[0], strerror(errno));
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("error");
        return -1;
    }

    //Macros -return the status code, otherwise -1 
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;


}

int main(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        int arg_count = tokenize(line, args);
        if (arg_count == 0) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        execute_command(args);
    }

    return 0;
}

