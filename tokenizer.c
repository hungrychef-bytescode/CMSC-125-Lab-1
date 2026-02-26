#include <string.h>     //for strtok
#include "shell.h"

void tokenize(char *input_dup, char *tokens[]) {
    int count = 0;

    char *token = strtok(input_dup, " \t\n");             //tokenize by whitespace

    if (token == NULL) {                               //if no tokens found, return empty array
        tokens[0] = NULL;
        return;
    }

    while (token != NULL) {
        tokens[count] = token;
        token = strtok(NULL, " \t\n");
        count++;
    }
    tokens[count] = NULL;               //null-terminate arr of token
}