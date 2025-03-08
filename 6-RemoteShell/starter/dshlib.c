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
#define DRAGON "dragon"
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX);
    if (!cmd_buff) {
        return ERR_MEMORY;
    }

    command_list_t clist;
    int rc = 0;

    while(1) {
        //Print's the prompt
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

        //Parse command line into command list
        rc = build_cmd_list(cmd_buff, &clist);

        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
        }
        else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }
        else if (rc == OK) {
            //Check if there are any commands to execute
            if (clist.num > 0) {
                // Check if first command is built-in
                Built_In_Cmds result = match_command(clist.commands[0].argv[0]);
                if (result != BI_NOT_BI) {
                    //Execute built-in command
                    result = exec_built_in_cmd(&clist.commands[0]);
                    if (result == BI_CMD_EXIT) {
                        break;
                    }
                } else {
                    //Execute piped commands
                    rc = execute_pipeline(&clist);
                    if (rc != OK) {
                        printf("Error: Pipeline execution failed\n");
                    }
                }
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

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    int rc;
    
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
    char *save_ptr;
    char *cmd_str = strtok_r(cmd_line_copy, PIPE_STRING, &save_ptr);
    
    while (cmd_str != NULL && clist->num < CMD_MAX) {
        //Trim leading/trailing spaces
        while(isspace(*cmd_str)) cmd_str++;
        char *end = cmd_str + strlen(cmd_str) - 1;
        while(end > cmd_str && isspace(*end)) end--;
        *(end + 1) = '\0';

        //Skip empty commands
        if (strlen(cmd_str) == 0) {
            cmd_str = strtok_r(NULL, PIPE_STRING, &save_ptr);
            continue;
        }

        //Parse the command into the cmd_buff structure
        rc = build_cmd_buff(cmd_str, &clist->commands[clist->num]);
        if (rc != OK) {
            return rc;
        }
        
        clist->num++;
        cmd_str = strtok_r(NULL, PIPE_STRING, &save_ptr);
    }

    //Check if we hit the command limit
    if (cmd_str != NULL) {
        return ERR_TOO_MANY_COMMANDS;
    }

    return clist->num > 0 ? OK : WARN_NO_CMDS;
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
    //Check if the input is the exit command
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }
    //Check if the input is the cd command
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    //Check if the input is the dragon command
    if (strcmp(input, DRAGON) == 0) {
        return BI_CMD_DRAGON;
    }
    //If the input is not a built-in command, return BI_NOT_BI
    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd) {
    //Fork a new process
    pid_t process_id = fork();
    
    //Check if the fork failed
    if (process_id == -1) {
        perror("fork");
        return ERR_EXEC_CMD;
    }
    //If the fork succeeded, execute the command
    else if (process_id == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    //If the fork succeeded, wait for the command to finish
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

int execute_pipeline(command_list_t *clist) {
    //Creates a 2d array of pipes which is one less than the number of commands
    int pipes[CMD_MAX-1][2];

    //Creates an array of process IDs
    pid_t pids[CMD_MAX];
    
    //Set up pipes for each command
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    //Fork a new process for each command
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        
        if (pids[i] == 0) {
            //If the command is not the first command, redirect the input from the previous command
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            //If the command is not the last command, redirect the output to the next command
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            //Execute the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    //Close the pipes
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    //Wait for all the commands to finish
    int status;
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return ERR_EXEC_CMD;
        }
    }
    
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    cmd_lst->num = 0;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL) {
        return OK;
    }
    
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }

    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = false;
    
    return OK;
}