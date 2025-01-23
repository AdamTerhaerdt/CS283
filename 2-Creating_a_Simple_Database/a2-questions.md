## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**:  I agree that adding the function `get_student(...)` is a good design choice because it reduces code duplication and makes the code more readable. In my implementation of this program I used `get_student(...)` to check if a student already exists before either adding or deleting a student. This helped me reduce the amount of code that I needed to write. What makes for a good design choice is making sure code is modular and reusable, to which this function is a good example.

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** The above implementation is a very bad idea because it introduces a bug that can be hard to identify at runtime. The reason why there is a bug is because the `student` structure is allocated on the stack when the function is called. After the function returns, the stack frame is then deallocated, which means that the `student` structure is pointing to invalid memory. If this pointer is used after the function returns, it could result in unexpected behavior. The reason it can be hard to identify is because C handles memory allocation and deallocation in this case. Using pass by reference makes it clear and easy to understand what is happening.

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** This implementation is better than the previous `2.` because `malloc()` is used to allocate memory on the heap and not the stack. The memory allocated using `malloc()` is not deallocated when the function return and persists on the heap until the memory is freed. Also the function correctly handles the case where `malloc()` fails to allocate memory by returning `NULL`. This function would be work because it correctly allocates memory for the student record and returns a pointer to it. There are a few potential problems with this implementation. The first being that the caller of this functions needs to remember that they need to free the memory allocated by `malloc()`. If the caller forgets to free the memory, it could result in memory leaks. In larger codebases this could be hard to manage. The second problem is that if malloc fails to allocate memory, the function will return `NULL`. This normally would not be an issue, but considering that the function also returns `NULL` when a student is not found, it could be hard to determine if the function returned `NULL` because the student was not found or because `malloc()` failed to allocate memory.

4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** This is an important distinction to make as a systems programmer. The `ls` command returns back the logical file size which represents the actual data size of a file. The `lseek` system call can be used to create "holes" within a file which is seeking to a position beyond EOF, writing data, and then leaving the gap with unallocated zeros. After adding student with `ID=1` the size is 128 bytes because the student record is 64 bytes and `ID=1` is stored at offset 1 * 64 bytes. This now equals (64 bytes + 64 bytes) =  128 bytes. After adding a student with `ID=3` the size is 256 bytes because again, the student record is 64 bytes and `ID=3` is stored at offset 3 * 64 bytes + the size of the student record (64 bytes). This now equals 256 bytes. After adding a student with `ID=64` the size is 4160 bytes because the student record is 64 bytes and `ID=64` is stored at offset 64 * 64 bytes plus the size of the student record (64 bytes). This now equals 4160 bytes. After create the student with `ID=64` the file size now equals 4160 bytes, and the `ls` command will show this size as it shows the logical file size including the holes that were created by the `lseek` system call.

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** The `du` command works differently than the `ls` command. The `du` command returns back the actual storage that is used on the disk. When we added the student with `ID=1`, `ID=3`, and `ID=63` the storage used on the disk remained unchanged because the student records were stored in consecutive blocks of 4096 bytes (4K). Our system uses a fixed filesystem blocks of 4096 bytes. Since student with `ID=63` fit into the first block, the storage used on the disk remained unchanged. Then, when we added student with `ID=64`, the system had to allocate a new block of 4096 bytes to store the student record. The student with `ID=64` took up 64 * 64 bytes = 4096 bytes + 64 bytes (size of the student record) = 4160 bytes, which is greater than 4096 bytes. This means that the system had to allocate a new block of 4096 bytes to store the student record. The storage used on the disk increased from 4K to 8K because the new block of 4096 bytes was added to the storage used on the disk.

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:** This is because the `ls` command returns the logical file size, which represents the actual data size of the file. The `du` command returns the actual storage used on the disk. When we added the student with `ID=99999`, the file size increased to 640K bytes because the student record is 64 bytes and `ID=99999` is stored at offset 99999 * 64 bytes = 6399936 bytes + 64 bytes for the student record. This now equals 640K bytes. The `du` command returns 12K because the actual storage used on the disk is 12K bytes. The `du` command only shows the actual storage used on the disk. It took 4K bytes to store the student records with `ID=1`, `ID=3`, and `ID=63`. It added another 4k bytes to store the student with `ID=64`. Finally, the system added another 4k bytes to store the student with `ID=99999`. This now equals (4K + 4K + 4K) = 12K bytes.