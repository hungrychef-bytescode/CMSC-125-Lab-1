#include <string.h>     //for strtok
#include <stdio.h>      //for printf
#include "shell.h"

/* tokenize func -> splits input string into tokens based on whitespace
   input_dup -> duplicate of orig input string. being modified by strtok
   tokens -> array of pointers storing tokenized strings
*/
void tokenize(char *input_dup, char *tokens[]) {
    int count = 0;

    char *token = strtok(input_dup, " \t\n");            //split input by space, tab, newline

    if (token == NULL) {                               //if no tokens found, return empty array
        tokens[0] = NULL;
        return;
    }

    while (token != NULL) {
        tokens[count] = token;
        token = strtok(NULL, " \t\n");
        count++;
        printf("Token %d: %s\n", count, tokens[count-1]);
    }
    tokens[count] = NULL;               //null-terminate arr of token
}