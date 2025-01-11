#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
int  reverse_string(char *, int, int);
int  word_print(char *, int, int);
int  replace_string(char *, int, int, char *, char *);


int setup_buff(char *buff, char *user_str, int len){
    if (!buff || !user_str || len <= 0) {
        return -2;
    }

    //Count string length using pointer arithmetic
    char *str_ptr = user_str;
    int str_len = 0;
    while (*str_ptr != '\0') {
        str_len++;
        str_ptr++;
    }

    //Check if string is too big for buffer
    if (str_len > len) {
        return -1;
    }

    //Index for the buffer
    int buff_index = 0;

    //Flag to check if the previous character was a whitespace.
    //Start's as 1 to handle leading spaces
    int previous_whitespace = 1;
    
    //Process the string until either the null terminator is reached
    //or the buffer is full.
    while (*user_str != '\0' && buff_index < len) {
        if (*user_str == ' ' || *user_str == '\t') {
            //Only add a space if previous char wasn't a space
            if (!previous_whitespace) {
                buff[buff_index] = ' ';
                buff_index++;
                previous_whitespace = 1;
            }
        }
        else {
            //Copy non-whitespace char
            buff[buff_index] = *user_str;
            buff_index++;
            previous_whitespace = 0;
        }
        user_str++;
    }
    
    //Fill remainder with dots
    while (buff_index < len) {
        buff[buff_index] = '.';
        buff_index++;
    }
    return buff_index;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int wc = 0;
    //Flag to check if word has started
    bool word_start = false;

    //Overflow detection
    if (str_len > len){
        return -1;
    }

    for(int i = 0; i < str_len; i++) {
        if(buff[i] == ' ' || buff[i] == '\t') {
            word_start = false;
        }
        else if(!word_start) {
            //Found start of new word
            wc++;
            word_start = true;
        }
    }
    return wc;
}
int reverse_string(char *buff, int len, int str_len){
    int start_index = 0;
    int end_index = str_len - 1;
    char tmp_char;
    
    //Overflow detection
    if (str_len > len){
        return -1;
    }

    //Only reverse up to the last non-dot character
    while (end_index >= 0 && buff[end_index] == '.') {
        end_index--;
    }

    //Add null terminator after last non-dot character
    buff[end_index + 1] = '\0';


    while (start_index < end_index) {
        //Swap characters
        tmp_char = buff[start_index];
        buff[start_index] = buff[end_index]; 
        buff[end_index] = tmp_char;
        start_index++;
        end_index--;
    }

    return str_len;
}
int word_print(char *buff, int len, int str_len){
    int wc = 0;
    bool word_start = false;
    int word_len = 0;

    //Overflow detection
    if (str_len > len){
        return -1;
    }

    //Find end of user string and add null terminator
    int end_index = str_len - 1;
    while (end_index >= 0 && buff[end_index] == '.') {
        end_index--;
    }
    buff[end_index + 1] = '\0';

    for(int i = 0; i < end_index + 1; i++) {
        if(buff[i] == ' ' || buff[i] == '\t') {
            if(word_start) {
                printf(" (%d)\n", word_len);
                word_len = 0;
            }
            word_start = false;
        }
        else if(!word_start) {
            wc++;
            word_start = true;
            printf("%d. %c", wc, buff[i]);
            word_len = 1;
        }
        else {
            printf("%c", buff[i]);
            word_len++;
        }
    }

    //Print length of last word if there was one
    if(word_start) {
        printf(" (%d)\n", word_len);
    }

    return wc;
}

int replace_string(char *buff, int len, int str_len, char *search, char *replace) {
    //TODO: Implement replace_string
    return 0;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    //This is safe because 'argc < 2' check comes first in the OR condition.
    //If 'argc < 2' is true, the program will exit with a return code of 1
    //without checking the second condition. This ensures that the program
    //will not attempt to access argv[1] if it does not exist. If it is greater
    //than 2, then we know that argv[1] exists and we can safely access it as
    //this is condition will be evaluated after the first condition.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //The purpose of the if statement below is the check if
    //the user has provided enough command line arguments for
    //the program to function properly. If it is less than three
    //arguments, then the program will print the usage message
    //and exit with a return code of 1.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99

    buff = malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        printf("Error: Failed to allocate memory for buffer\n");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;  
        case 'r':
            rc = reverse_string(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error reversing string, rc = %d", rc);
                exit(2);
            }
            printf("Reversed string: %s\n", buff);
            break;
            
        case 'w':
            rc = word_print(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error printing words, rc = %d", rc);
                exit(2);
            }
            break;
        case 'x':
            if (argc < 5) {
                printf("Error: replace option requires search and replace strings\n");
                usage(argv[0]);
                exit(1);
            }
            rc = replace_string(buff, BUFFER_SZ, user_str_len, argv[3], argv[4]);
            if (rc < 0){
                printf("Error replacing words, rc = %d", rc);
                exit(2);
            }
            printf("Modified string: %s\n", buff);
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//
//          It is good practice to provide both the pointer and the
//          length because it allows the function to be reused in
//          differnet situations. For example, if we wanted to use
//          set up buff for other buffers that are different sizes
//          we can now pass in the size of those buffers as well.
//          It does not tie down the function to using any one specific
//          buffer size. It is also good to include the length as well
//          as the buffer pointer because it makes it more clear to
//          the developer what the expected buffer size is. Finally,
//          it allows for better overflow preventation as functions
//          can easily check the length of the buffer.
