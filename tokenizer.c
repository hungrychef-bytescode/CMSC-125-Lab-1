#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

void tokenize(char *input, char *tokens[]) {
    int i = 0;                      //index for input
    int t = 0;                      //index for tokens

    while (input[i] != '\0') {

        while (input[i] == ' ' || input[i] == '\t') {       //skip whitespace
            i++;
        }     
            
        if (input[i] == '\0')
            break;

        //handle quoted strings
        if (input[i] == '"') {
            i++;                            // skip opening quote
            int start = i;
            
            while (input[i] != '"' && input[i] != '\0') {
                i++;
            }

            int length = i - start;
            tokens[t] = malloc(length + 1);
            strncpy(tokens[t], &input[start], length);
            tokens[t][length] = '\0';

            t++;

            if (input[i] == '"')
                i++;                                              // skip closing quote
        } else if (input[i] == '>' && input[i+1] == '>') {        //append >> operator
            tokens[t++] = strdup(">>");
            i += 2;
        } else if (input[i] == '<' || input[i] == '>' || input[i] == '&') {   //one character operators
            char op[2] = {input[i], '\0'};
            tokens[t++] = strdup(op);
            i++;
        } else {                                                            //normal
            int start = i;
            while (input[i] != '\0' && input[i] != ' ' &&
                   input[i] != '\t' && input[i] != '<' &&
                   input[i] != '>' && input[i] != '&') {
                i++;
            }
            int length = i - start;
            tokens[t] = malloc(length + 1);
            strncpy(tokens[t], &input[start], length);
            tokens[t][length] = '\0';
            t++;
        }
        if (t >= MAX_ARGS - 1)
            break;
    }
    tokens[t] = NULL;
}

void free_tokens(char *tokens[]) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
}