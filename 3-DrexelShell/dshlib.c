#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    //Use memset to initialize the command list (set all values to 0)
    memset(clist, 0, sizeof(command_list_t));
    
    //Check for empty command
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    // Make a copy of cmd_line since strtok modifies the string
    char cmd_line_copy[SH_CMD_MAX];
    strcpy(cmd_line_copy, cmd_line);

    // Split commands by pipe character
    char *cmd_str = strtok(cmd_line_copy, PIPE_STRING);
    while (cmd_str != NULL && clist->num < CMD_MAX) {
        printf("DEBUG: Found command: '%s'\n", cmd_str);
        
        // Trim leading/trailing spaces
        while(isspace(*cmd_str)) cmd_str++;
        char *end = cmd_str + strlen(cmd_str) - 1;
        while(end > cmd_str && isspace(*end)) end--;
        *(end + 1) = '\0';

        // Skip empty commands
        if (strlen(cmd_str) == 0) {
            cmd_str = strtok(NULL, PIPE_STRING);
            continue;
        }

        // Make a copy of the command for argument parsing
        char cmd_copy[SH_CMD_MAX];
        strncpy(cmd_copy, cmd_str, SH_CMD_MAX - 1);
        cmd_copy[SH_CMD_MAX - 1] = '\0';

        // Get executable name (first token)
        char *token = strtok(cmd_copy, " ");
        if (token != NULL) {
            if (strlen(token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[clist->num].exe, token);

            // Get remaining tokens as arguments
            char args[ARG_MAX] = "";
            token = strtok(NULL, " ");
            while (token != NULL) {
                if (strlen(args) + strlen(token) + 2 >= ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                if (strlen(args) > 0) {
                    strcat(args, " ");
                }
                strcat(args, token);
                token = strtok(NULL, " ");
            }
            strcpy(clist->commands[clist->num].args, args);
        }

        printf("DEBUG: Added command %d: %s\n", clist->num, clist->commands[clist->num].exe);
        clist->num++;
        cmd_str = strtok(NULL, PIPE_STRING);
        printf("DEBUG: Next command: %s\n", cmd_str);
    }

    if (clist->num >= CMD_MAX) {
        return ERR_TOO_MANY_COMMANDS;
    }

    return clist->num > 0 ? OK : WARN_NO_CMDS;
}