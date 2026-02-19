#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for exit
#include <string.h>     //for strcmp
#include <unistd.h>     //for chdir
#include "shell.h"      

void executor(Command *cmd){

     if (cmd == NULL || cmd->command == NULL)
        return;

    // built-in commands
    if (strcmp(cmd->command, "exit") == 0) {            //exit shell
        printf("exiting shell......\n");
        exit(0);
    }

    if (strcmp(cmd->command, "cd") == 0) {              //change directory
        if (cmd -> args[1] == NULL) {
            fprintf(stderr, "mysh: error: \"cd\" requires an argument\n");
        } else if (chdir(cmd->args[1]) == 0) {
            printf("changed directory");
        } else {
            perror("failed to change directory");
        }
    }

    if (strcmp(cmd->command, "pwd") == 0) {             //print current working directory
        char cwd[1024];     
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("failed to get current directory");
        }
    }
}