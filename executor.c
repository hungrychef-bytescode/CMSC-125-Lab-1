#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for exit
#include <string.h>     //for strcmp
#include <unistd.h>     //for chdir
#include "shell.h"      

void executor(Command *cmd){

     if (cmd == NULL || cmd->command == NULL)
        return;

    if (strcmp(cmd->command, "exit") == 0) {
        exit(0);
    }
    if (strcmp(cmd->command, "cd") == 0) {
        if (cmd -> args[1] == NULL) {
            fprintf(stderr, "mysh: error: \"cd\" requires an argument\n");
        } else if (chdir(cmd->args[1]) != 0) {
            perror("mysh: error: failed to change directory");
        }
    }
}