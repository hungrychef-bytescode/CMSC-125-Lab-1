#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

/* ============================================================================
 * PRIVATE HELPER FUNCTIONS
 * ============================================================================ */

static void trim_whitespace(char *str);
static int tokenize_input(char *line, char **tokens);
static int process_tokens(char **tokens, int token_count, Command *cmd);


/* ============================================================================
 * PUBLIC INTERFACE - Command Management
 * ============================================================================ 
 * Initialize a Command structure to safe default values
 */
void init_command(Command *cmd) {
    cmd->command = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->background = false;
    
    for (int i = 0; i < MAX_ARGS; i++) {
        cmd->args[i] = NULL;
    }
}

/**
 * Clean up a Command structure
 * Does not free individual strings as they point to the original input buffer.
 */
void free_command(Command *cmd) {
    init_command(cmd);
}

int parse_command(char *line, Command *cmd) {
    init_command(cmd);
    trim_whitespace(line);
    
    if (strlen(line) == 0) {
        return -1;
    }
    
    char *tokens[MAX_ARGS];
    int token_count = tokenize_input(line, tokens);
    
    if (token_count == 0) {
        return -1;
    }
    
    if (token_count < 0) {
        fprintf(stderr, "mysh: error: too many arguments\n");
        return -1;
    }
    
    if (process_tokens(tokens, token_count, cmd) < 0) {
        return -1;
    }
    
    if (cmd->command == NULL) {
        fprintf(stderr, "mysh: error: no command specified\n");
        return -1;
    }
    
    return 0;
}


/* ============================================================================
 * INPUT PREPROCESSING - Whitespace Handling
 * ============================================================================ 
 * Trim leading and trailing whitespace from string
 */

static void trim_whitespace(char *str) {
    if (str == NULL) return;
    
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    if (*start == '\0') {
        *str = '\0';
        return;
    }
    
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    
    *(end + 1) = '\0';
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}


/* ============================================================================
 * TOKENIZATION - Breaking Input into Tokens
 * ============================================================================
 * Tokenize input string by whitespace characters
 * Uses strtok() to split input. Note: strtok() modifies the input string.
 */

static int tokenize_input(char *line, char **tokens) {
    int count = 0;
    char *token = strtok(line, " \t\n\r");
    
    while (token != NULL && count < MAX_ARGS - 1) {
        tokens[count++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    
    if (token != NULL) {
        return -1;
    }
    
    tokens[count] = NULL;
    return count;
}


/** ===========================================================================
 * TOKEN PROCESSING - Building Command Structure
 * ============================================================================ 
 * Classifies tokens: operators (<, >, >>, &) vs arguments
 * Validates: single I/O redirections, & as last token, command exists
 */

static int process_tokens(char **tokens, int token_count, Command *cmd) {
    int arg_index = 0;
    int i = 0;
    
    while (i < token_count) {
        char *token = tokens[i];
        
        if (strcmp(token, "&") == 0) {
            cmd->background = true;
            if (i != token_count - 1) {
                fprintf(stderr, "mysh: error: '&' must be at end of command\n");
                return -1;
            }
            i++;
            continue;
        }
        
        if (strcmp(token, "<") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '<'\n");
                return -1;
            }
            if (cmd->input_file != NULL) {
                fprintf(stderr, "mysh: error: multiple input redirections\n");
                return -1;
            }
            cmd->input_file = tokens[i];
            i++;
            continue;
        }
        
        if (strcmp(token, ">>") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '>>'\n");
                return -1;
            }
            if (cmd->output_file != NULL) {
                fprintf(stderr, "mysh: error: multiple output redirections\n");
                return -1;
            }
            cmd->output_file = tokens[i];
            cmd->append = true;
            i++;
            continue;
        }
        
        if (strcmp(token, ">") == 0) {
            i++;
            if (i >= token_count) {
                fprintf(stderr, "mysh: error: missing filename after '>'\n");
                return -1;
            }
            if (cmd->output_file != NULL) {
                fprintf(stderr, "mysh: error: multiple output redirections\n");
                return -1;
            }
            cmd->output_file = tokens[i];
            cmd->append = false;
            i++;
            continue;
        }
        
        if (arg_index >= MAX_ARGS - 1) {
            fprintf(stderr, "mysh: error: too many arguments\n");
            return -1;
        }
        
        cmd->args[arg_index++] = token;
        if (cmd->command == NULL) {
            cmd->command = token;
        }
        i++;
    }
    
    cmd->args[arg_index] = NULL;
    if (cmd->command == NULL) {
        fprintf(stderr, "mysh: error: no command found\n");
        return -1;
    }
    
    return 0;
}
