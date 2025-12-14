#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() 
{
    int fd1[2];  // Parent → Child
    int fd2[2];  // Child → Parent
    
    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    
    char input_str[100];
    pid_t p;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    // Parent prompt (1st input)
    printf("Input : ");
    scanf("%s", input_str);

    p = fork();

    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    /* ============================
       Parent Process (P1)
       ============================ */
    if (p > 0) 
    {
        char buffer[200];
        close(fd1[0]);  // Parent writes to fd1
        close(fd2[1]);  // Parent reads from fd2

        // Send initial input to child
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        wait(NULL);

        // Read back final string from child
        read(fd2[0], buffer, sizeof(buffer));
        close(fd2[0]);

        // Parent must append "gobison.org"
        strcat(buffer, fixed_str2);

        // Final output (only parent prints this)
        printf("Output : %s\n", buffer);
    }

    /* ============================
       Child Process (P2)
       ============================ */
    else 
    {
        char buffer[200];
        char input2[100];

        close(fd1[1]);  // Child reads from fd1
        close(fd2[0]);  // Child writes to fd2

        // Read initial string from parent
        read(fd1[0], buffer, sizeof(buffer));
        close(fd1[0]);

        // Append "howard.edu"
        strcat(buffer, fixed_str1);

        // Child prints intermediate result
        printf("Output : %s\n", buffer);

        // Child prompt (2nd input)
        printf("Input : ");
        scanf("%s", input2);

        // Append second user input
        strcat(buffer, input2);

        // Send back to parent
        write(fd2[1], buffer, strlen(buffer) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}
