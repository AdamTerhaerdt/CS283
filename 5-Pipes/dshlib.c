#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include "dragon.h"

int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX);
    if (!cmd_buff) {
        return ERR_MEMORY;
    }

    cmd_buff_t cmd;
    int rc = 0;

    while(1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        //Remove the trailing newline
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        //Check for exit command before any other processing
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }

        //Parse command line into command buffer
        rc = build_cmd_buff(cmd_buff, &cmd);

        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
        }
        else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }
        else if (rc == OK) {
            Built_In_Cmds result = exec_built_in_cmd(&cmd);
            if (result == BI_CMD_EXIT) {
                break;
            }
        }
    }

    free(cmd_buff);
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd)
{
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    //Initialize cmd buffer
    memset(cmd, 0, sizeof(cmd_buff_t));

    //Trim leading/trailing spaces
    while(isspace(*cmd_line)) cmd_line++;
    char *end = cmd_line + strlen(cmd_line) - 1;
    while(end > cmd_line && isspace(*end)) end--;
    *(end + 1) = '\0';

    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    char cmd_copy[SH_CMD_MAX];
    strcpy(cmd_copy, cmd_line);
    char *p = cmd_copy;
    bool in_quotes = false;
    char *token_start = p;

    //Checking for quotes and spaces
    while (*p != '\0' && cmd->argc < ARG_MAX) {
        if (*p == '"') {
            in_quotes = !in_quotes;
            if (in_quotes) {
                token_start = p + 1;
            } else {
                *p = '\0';
                if (token_start != p) {
                    cmd->argv[cmd->argc] = strdup(token_start);
                    cmd->argc++;
                }
                token_start = p + 1;
            }
        } else if (isspace(*p) && !in_quotes) {
            *p = '\0';
            if (token_start != p) {
                cmd->argv[cmd->argc] = strdup(token_start);
                cmd->argc++;
            }
            token_start = p + 1;
            while (isspace(*(p + 1))) p++;
        }
        p++;
    }

    //Handle last token if any
    if (!in_quotes && token_start < p && *token_start != '\0') {
        cmd->argv[cmd->argc] = strdup(token_start);
        cmd->argc++;
    }

    //Add null terminator for execvp
    cmd->argv[cmd->argc] = NULL;

    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds type = match_command(cmd->argv[0]);
    
    //Switch case to handle different built-in commands
    switch(type) {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
            
        case BI_CMD_CD:
            if (cmd->argc == 1) {
                return BI_EXECUTED;
            }
            if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                    return BI_RC;
                }
                return BI_EXECUTED;
            }
            printf("cd: Invalid number of arguments\n");
            return BI_RC;
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
        //Default to the `exec_cmd()` 
        default:
            if (exec_cmd(cmd) != OK) {
                printf("Error: Not a valid command\n");
                return BI_RC;
            }
            return BI_EXECUTED;
    }
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    if (strcmp(input, DRAGON) == 0) {
        return BI_CMD_DRAGON;
    }
    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t process_id = fork();
    
    if (process_id == -1) {
        perror("fork");
        return ERR_EXEC_CMD;
    }
    else if (process_id == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            return exit_code;
        }
    }
    return OK;
}