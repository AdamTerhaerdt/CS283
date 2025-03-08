1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: The reason why we call `fork()` before calling `execvp()` is because if we were just to call `execvp()` directly, it would replace the current process running with a new process. Our shell would be replaced by the command and then terminate. The reason why we call `fork()` is because it creates a new child process that runs the command, and then the parent process will wait for the child process to finish before continuing. This allows us to handle the output and exit codes.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If the `fork()` system call fails, then the function should return -1 to indicate an error. In my code I handle this scenario by checking if the `process_id` is -1, and if it is, then the function will use `perror()` to print an error message to the user and then it will return an error code.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The `execvp()` function is able to search through directories using the `PATH` environment variable. The `PATH` env variable is a list of directories which the `execvp()` function will search through to find the command to execute.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling `wait()` in the parent process after forking is because the program needs to wait for the child process to finish before it can continue. This allows the parent process to handle the output and exit code of the child process. If we did not call `wait()`, then the parent process would not have enough information to know about the child process output and exit code.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` is very important because it allows the parent process to get the exit status of the child process. This is imporant because it gives the parents process the ability to handle output and exit codes of its child processes.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: This code took me a while to figure out. How my program handles quoted arguments is that there is a `while` loop checking to see for the null terminator or if we reached ARG_MAX. While we have not, it will check if the current character is in a quote. If it is, then set the `in_quotes` boolean to true. If not, then it will check if the current char is a space and if `in_quotes` is false. If it is, then we have reached the end of the command and we can break out of the loop. If we are not in a quote, then we will copy the command into a new array and then increment the `argc` variable. This is necessary because we our shell needs to be able to handle commands that are quoted and have spaces in them.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: Instead of directly using `comamand_list_t` to store the commands, I instead used `cmd_buff_t` to store the command. I also did not need a `while` loop to iterate through the commands anymore and I did not need to use `strtok_r()` to split the commands by the delimiter `PIPE_STRING`. The challenge that I experienced when refactoring the code was changing to the new data structure and making sure that the command was being stored correctly.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  From my research, I found that Linux signals are a way for a process to handle asynchronous events. They are different than other forms of interprocess communication because signals are implemented at the kernel level, while other forms of IPC are not. Signals are a one way notification system that notify a process that an event has occurred, and signals have no persistence beyond delivery.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: `SIGKILL` is used to forcefully terminate a process. It should be used a last resort and is a quick way to stop a process. `SIGTERM` is also used to terminate a process, but it allows the process to cleanup before exiting. `SIGTERM` is more graceful than `SIGKILL`. `SIGINT` is sent to the process when the user presses `Ctrl+C`. It also allows for graceful termination.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: When a process receives `SIGSTOP`, the process is stopped and cannot continue until it is continued. `SIGSTOP` cannot be caught or ignored like `SIGINT`. The reason why it cannot be ignored is because we want to ensure that the kernel maintains control over the process and is critical for systems stability and secutiry.