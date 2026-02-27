#include <string.h>     //for strncpy
#include <stdio.h>      //for fprintf
#include "shell.h"

/* tokenize func -> splits input string into tokens based on whitespace
   handles quoted strings as a single token (e.g. "hello world" -> one token)
   handles quotes in the middle of a word (e.g. hel"lo" -> hello)
   reports unclosed quotes and empty tokens as errors
   input_dup -> duplicate of orig input string
   tokens -> array of pointers storing tokenized strings
*/

// static pool so tokens stay valid for the lifetime of the command
// reused each call since only one command is processed at a time
static char token_pool[MAX_ARGS][4096];

void tokenize(char *input_dup, char *tokens[]) {
    int count = 0;
    char *p = input_dup;

    while (*p != '\0' && count < MAX_ARGS - 1) {

        // skip leading whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n') p++;

        if (*p == '\0') break;

        // build current token into pool buffer, handling mid-word quotes
        char *buf = token_pool[count];
        int buf_len = 0;
        int in_token = 0;

        while (*p != '\0') {
            if (*p == '"' || *p == '\'') {
                // quoted section: absorb everything until matching closing quote
                char quote = *p++;      // skip opening quote
                in_token = 1;

                while (*p != '\0' && *p != quote) {
                    if (buf_len < 4095) buf[buf_len++] = *p;
                    p++;
                }

                if (*p == quote) {
                    p++;                // skip closing quote
                } else {
                    fprintf(stderr, "Error: Unclosed Quote\n");
                    tokens[0] = NULL;
                    return;
                }

            } else if (*p == ' ' || *p == '\t' || *p == '\n') {
                break;                  // whitespace ends token
            } else {
                if (buf_len < 4095) buf[buf_len++] = *p;
                p++;
                in_token = 1;
            }
        }

        if (!in_token) continue;

        buf[buf_len] = '\0';

        // empty quotes (e.g. "" or '') produce empty string — report and stop
        if (buf_len == 0) {
            fprintf(stderr, "Error: Empty Token\n");
            tokens[0] = NULL;
            return;
        }

        tokens[count++] = buf;
    }

    tokens[count] = NULL;               //null-terminate arr of token
}