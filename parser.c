#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

/* ============================================================================
 * PRIVATE HELPER FUNCTIONS
 * ============================================================================ */

static void trim_whitespace(char *str);
static int tokenize_input(char *line, char tokens[][MAX_TOKEN_LEN], int *count);
static int process_tokens(char tokens[][MAX_TOKEN_LEN], int token_count, Command *cmd);


/* ============================================================================
 * PUBLIC INTERFACE - Command Management
 * ============================================================================
 * Initialize a Command structure to safe default values.
 * No pointer fields — all data lives inside the struct itself.
 */
void init_command(Command *cmd) {
    cmd->command[0]     = '\0';
    cmd->input_file[0]  = '\0';
    cmd->output_file[0] = '\0';
    cmd->append         = false;
    cmd->background     = false;
    cmd->arg_count      = 0;

    for (int i = 0; i < MAX_ARGS; i++) {
        cmd->args[i][0] = '\0';
    }
}

/**
 * Reset a Command structure back to defaults.
 * Nothing to free — all memory is inside the struct.
 */
void free_command(Command *cmd) {
    init_command(cmd);
}

/**
 * Parse a raw input line into a Command structure.
 * Returns 0 on success, -1 on error or empty input.
 */
int parse_command(char *line, Command *cmd) {
    init_command(cmd);
    trim_whitespace(line);

    if (strlen(line) == 0) {
        return -1;
    }

    char tokens[MAX_ARGS][MAX_TOKEN_LEN];
    int token_count = 0;

    if (tokenize_input(line, tokens, &token_count) < 0) {
        fprintf(stderr, "mysh: error: too many arguments\n");
        return -1;
    }

    if (token_count == 0) {
        return -1;
    }

    if (process_tokens(tokens, token_count, cmd) < 0) {
        free_command(cmd);
        return -1;
    }

    if (cmd->command[0] == '\0') {
        fprintf(stderr, "mysh: error: no command specified\n");
        free_command(cmd);
        return -1;
    }

    return 0;
}


/* ============================================================================
 * INPUT PREPROCESSING - Whitespace Handling
 * ============================================================================
 * Trim leading and trailing whitespace in-place.
 * The char *str parameter is just a function argument — not a heap pointer.
 */
static void trim_whitespace(char *str) {
    if (str == NULL) return;

    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}


/* ============================================================================
 * TOKENIZATION - Breaking Input into Tokens
 * ============================================================================
 * Splits line by whitespace into a 2D array of tokens.
 * No heap allocation — tokens is a fixed 2D char array on the stack.
 * Returns 0 on success, -1 if token count exceeds MAX_ARGS.
 */
static int tokenize_input(char *line, char tokens[][MAX_TOKEN_LEN], int *count) {
    *count = 0;

    char *token = strtok(line, " \t\n\r");
    while (token != NULL) {
        if (*count >= MAX_ARGS - 1) {
            return -1;
        }

        strncpy(tokens[*count], token, MAX_TOKEN_LEN - 1);
        tokens[*count][MAX_TOKEN_LEN - 1] = '\0';

        (*count)++;
        token = strtok(NULL, " \t\n\r");
    }

    return 0;
}


/* ============================================================================
 * TOKEN PROCESSING - Building Command Structure
 * ============================================================================
 * Classifies each token as an operator or argument and fills the Command.
 * Uses strncpy — no strdup, no malloc, no heap allocation at all.
 */
static int process_tokens(char tokens[][MAX_TOKEN_LEN], int token_count, Command *cmd) {
    int arg_index = 0;
    int i = 0;

    while (i < token_count) {
        char *token = tokens[i]; /* local pointer for readability only, not stored */

        /* Background operator */
        if (strcmp(token, "&") == 0) {
            if (i != token_count - 1) {
                fprintf(stderr, "mysh: error: '&' must be at end of command\n");
                return -1;
            }
            cmd->background = true;
            i++;
            continue;
        }

        /* Input redirection */
        if (strcmp(token, "<") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '<'\n");
                return -1;
            }
            if (cmd->input_file[0] != '\0') {
                fprintf(stderr, "mysh: error: multiple input redirections\n");
                return -1;
            }
            strncpy(cmd->input_file, tokens[i], MAX_TOKEN_LEN - 1);
            cmd->input_file[MAX_TOKEN_LEN - 1] = '\0';
            i++;
            continue;
        }

        /* Append redirection */
        if (strcmp(token, ">>") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '>>'\n");
                return -1;
            }
            if (cmd->output_file[0] != '\0') {
                fprintf(stderr, "mysh: error: multiple output redirections\n");
                return -1;
            }
            strncpy(cmd->output_file, tokens[i], MAX_TOKEN_LEN - 1);
            cmd->output_file[MAX_TOKEN_LEN - 1] = '\0';
            cmd->append = true;
            i++;
            continue;
        }

        /* Truncate redirection */
        if (strcmp(token, ">") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '>'\n");
                return -1;
            }
            if (cmd->output_file[0] != '\0') {
                fprintf(stderr, "mysh: error: multiple output redirections\n");
                return -1;
            }
            strncpy(cmd->output_file, tokens[i], MAX_TOKEN_LEN - 1);
            cmd->output_file[MAX_TOKEN_LEN - 1] = '\0';
            cmd->append = false;
            i++;
            continue;
        }

        /* Regular argument */
        if (arg_index >= MAX_ARGS - 1) {
            fprintf(stderr, "mysh: error: too many arguments\n");
            return -1;
        }

        strncpy(cmd->args[arg_index], token, MAX_TOKEN_LEN - 1);
        cmd->args[arg_index][MAX_TOKEN_LEN - 1] = '\0';

        /* First argument is the command name */
        if (cmd->command[0] == '\0') {
            strncpy(cmd->command, token, MAX_TOKEN_LEN - 1);
            cmd->command[MAX_TOKEN_LEN - 1] = '\0';
        }

        arg_index++;
        i++;
    }

    cmd->arg_count    = arg_index;
    cmd->args[arg_index][0] = '\0'; /* mark end of args */

    return 0;
}
