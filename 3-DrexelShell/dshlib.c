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
    //Initialize the command list to all zeros
    memset(clist, 0, sizeof(command_list_t));

    //Check for empty command line
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    //Copy the command line to a new string
    char cmd_line_copy[SH_CMD_MAX];
    strcpy(cmd_line_copy, cmd_line);

    //Split commands by pipe character
    char *save_ptr1;
    char *cmd_str = strtok_r(cmd_line_copy, PIPE_STRING, &save_ptr1);
    
    while (cmd_str != NULL) {
        
        //Trim leading/trailing spaces
        //Used this stackoverflow post
        //https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
        while(isspace(*cmd_str)) cmd_str++;
        char *end = cmd_str + strlen(cmd_str) - 1;
        while(end > cmd_str && isspace(*end)) end--;
        *(end + 1) = '\0';

        //Continue if the command is empty
        if (strlen(cmd_str) == 0) {
            cmd_str = strtok_r(NULL, PIPE_STRING, &save_ptr1);
            continue;
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Make a copy of cmd_str for tokenizing
        char cmd_copy[SH_CMD_MAX];
        strcpy(cmd_copy, cmd_str);

        char *save_ptr2;
        char *token = strtok_r(cmd_copy, " ", &save_ptr2);
        if (token != NULL) {
            //Store the exe in the command list
            strcpy(clist->commands[clist->num].exe, token);
            char args[ARG_MAX] = "";
            token = strtok_r(NULL, " ", &save_ptr2);

            //Go through each arg and store it in the command list
            while (token != NULL) {
                //Check if the arg is too big
                if (strlen(args) + strlen(token) + 2 > ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                //Add a space between args if there are any
                if (strlen(args) > 0) {
                    strcat(args, " ");
                }
                strcat(args, token);
                token = strtok_r(NULL, " ", &save_ptr2);
            }
            //Store the args in the command list
            strcpy(clist->commands[clist->num].args, args);
        }
        clist->num++;
        cmd_str = strtok_r(NULL, PIPE_STRING, &save_ptr1);
    }

    return OK;
}