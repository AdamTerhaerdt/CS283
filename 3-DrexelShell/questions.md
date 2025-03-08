1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  The function, `fgets()`, is a good choice for this application for many reason. First the is that the function reads the given number of charcters of a line from an input stream and stores it into a specified string. This is particularly useful for this program because we are reading in the user's input line by line.  Also `fgets()` has safety and predictability built in, instead of using `scanf()` which can be unsafe and unpredictable. Specifically, `fgets()` is less prone to buffer overflows and segmentation faults. You can specify the size of the buffer to read in, so you don't have to worry about reading in more than you need. Also `fgets()` reads the entire line of input, including spaces, which is useful to get the entire command from the user.

    I used this website to help me understand `fgets()`: https://www.geeksforgeeks.org/fgets-function-in-c/#

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  The reason we use `malloc()` instead of a fixed-sized array is because malloc allows us to allocate memory at runtime, instead of a fixed-sized array which would require us to know the maximum command length beforehand. `malloc()` allows for dynamic memory allocation, so we can allocate the exact amount of memory needed to the user's input. Also, `malloc()` persists until explicitly freed, so we don't have to worry about it going out of scope and being deleted.


3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  If we did not trim the spaces, the command would be stored with leading and trailing spaces, which would cause issues when executing the command. The command ` ls ` would be treated differently than `ls` because of the leading space. This would cause errors in our program because we are not properly handling the user's input. Trimming the spaces allows us to have consistent behavior across all commands.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: The first type of redirection we should implment in our shell is output redirection. This would allow us to redirect the output of a command to a file instead of using the terminal. An example of this command would be `ls -la > my_file.txt`. The next would be input rediction. This would allow us to redirect the input of a command to a file instead of using the terminal. An example of this would be `cat < my_file.txt`. Another redirection would be redirecting errors. This would allow us to redirect the error messages of a command to a file instead of using the terminal. An example of this would be `ls -la 2> my_file.txt`. A few challenges we might encounter are needing to parse `<`, `>`, and `2>` to check for redirection. Another challenege might be making sure that we have the correct file permissions to write or read from the file. Another challenege that we might encounter while implementing redirection is making sure that there is enough disk space to properly write the file. Finally, we might encounter is making sure that the file exists and is writable.
- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key difference between redirection and piping is that piping is used to connect the output of one command to the input of another command. Redirection is used to send output or take input from a file instead of the terminal. Piping creates the connection between commands, while redirection connects the command to a file.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: It is important to keep these messages seperate in the shell because it allows us to keep our regular output and error messages seperate from one another. `STDERR` will redirect error messages to a file, while `STDOUT` will redirect regular output to a file. This allows us to have a more organized and readable output. Also pipes only pass `STDOUT` to the next command, so if we want to pass `STDERR` to the next command we need to use redirection.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our custom shells should handle errors from commands that fail by printing the error message to the terminal. We could also consider having our error messages be printed to a file. In cases where we have a command that outputs both `STDOUT` and `STDERR`, we could use redirection. We could print the `STDERR` to the terminal and have the `STDERR` along with the `STDOUT` be redirected to a file. This way the user can easily see the error message in the terminal and can find the error along with the output in a file. 