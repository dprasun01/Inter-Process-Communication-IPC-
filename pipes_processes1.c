#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

int main() 
{ 
    int fd1[2];  // First pipe: P1 -> P2
    int fd2[2];  // Second pipe: P2 -> P1
    char fixed_str1[] = "howard.edu"; 
    char input_str[100], concat_str[200]; 
    pid_t p; 

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1; 
    }

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str);

    p = fork();

    if (p < 0) { 
        fprintf(stderr, "fork Failed");
        return 1; 
    } 
  
    // Parent Process (P1)
    else if (p > 0) { 
        close(fd1[0]); // Close read end of pipe1
        close(fd2[1]); // Close write end of pipe2

        // Write input to child
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close write end of pipe1

        wait(NULL); // Wait for the child to complete

        // Read the concatenated string from the child
        read(fd2[0], concat_str, 200);
        close(fd2[0]); // Close read end of pipe2

        // Prompt the user for the second input
        char second_input[100];
        printf("Enter another string to send back: ");
        scanf("%s", second_input);

        // Append the second input to the received string
        strcat(concat_str, second_input);
        printf("Final concatenated string: %s\n", concat_str);
    }

    // Child Process (P2)
    else { 
        close(fd1[1]); // Close write end of pipe1
        close(fd2[0]); // Close read end of pipe2

        // Read input from parent
        char temp_str[200];
        read(fd1[0], temp_str, 100);
        close(fd1[0]); // Close read end of pipe1

        // Concatenate fixed_str1 and print
        strcat(temp_str, fixed_str1);
        printf("Concatenated string: %s\n", temp_str);

        // Send the concatenated string back to the parent
        write(fd2[1], temp_str, strlen(temp_str) + 1); 
        close(fd2[1]); // Close write end of pipe2

        exit(0); 
    } 

    return 0; 
}
