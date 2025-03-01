1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

In my implementation, I call `waitpid()` on all child processes in the `execute_pipeline()` function. My code iterates through each process ID in the `pids` array and calls
`waitpid()` for each child process. This function only returns when the child process has finished executing. If you forget to call `waitpid()` on all child processes, the shell
will not wait for the child processes to finish executing and will go on to accept new user input. This will cause the shell to execute commands in parallel, which can result to
undefined behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

The reason why we need to close all unused pipes after calling `dup2()` is because if we don't close the pipe, the child process might never exit. This is because the pipe is
still open and the child process my never recieve a `EOF` signal to end the read end of the pipe. This will cause the child process to "hang" indefinitely. Also, if we don't close pipes, over time, the number of pipes will increase, which will cause the shell to run out of resources.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The reason why `cd` is implemented as a build-in command rather than an external command is because we don't want to create a child process to change directory. If we were to
fork this command into it's own child process, the child process would change it's working directory, but when we exit the child process, the parents processes directory will not change, making the command useless. External commands cannot modify the shell's environment. The reason why we implementn `cd` as a built-in command is so that it actually modifies the directory the user is expecting.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

The first action I would do to modify the implementation to allow an arbitrary number of piped commands is to modify the `command_list_t` structure to accept a buffer of commands. Then, in my `execute_pipeline` function, instead of hard coding the 2d array and the process_ids to a fixed array, I would use `malloc()` to dynamically set their sizes. To handle memory allocation efficiently, we would need to free the commands once executed. The trade-offs that we need to consider when doing this is memory management can be harder when doing it this way. Memory leaks are more of a potential. Also, the performance of the shell might drop as we add more commands. Finally, resource cleanup and management becomes harder when handling error cases.