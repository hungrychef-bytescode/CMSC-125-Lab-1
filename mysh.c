//mysh.c  simple shell implementation for CMSC 125 lab 1
#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for free
#include <string.h>     //for strcspn, strdup
#include "shell.h"      //for Command struct and function prototypes

#define MAX_INPUT 1024

/*
main function for the shell
entry point and used for the interactive loop
*/
int main() {

    char input[MAX_INPUT];
    char *tokens[MAX_ARGS];

    while (1){
        cleanup_background_jobs();

        printf("mysh> ");

        if (fgets(input, MAX_INPUT, stdin) == NULL) break;              //exit on error.

        input[strcspn(input, "\n")] = '\0';                             //remove excess newline

        char *input_dup = strdup(input);                                //duplicate input for tokenization

        tokenize(input_dup, tokens);                                    //tokenize input to tokens arr

        if (tokens[0] == NULL) {                                        //if empty, prompt again
            free(input_dup);
            continue;
        }

        Command cmd = parse_command(tokens);
        free(input_dup);                                                //free input dup string

        executor(&cmd);                                                 //call executor to run commands
        free_command(&cmd);
        }
    return 0;
}