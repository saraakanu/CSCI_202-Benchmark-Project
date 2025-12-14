#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        exit(1);
    }

    int pipefd[2], pipe2[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    if (pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /* ================================
           CHILD BLOCK → will create grep and sort
           ================================ */
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0) {
            /* ================================
               CHILD'S CHILD → P3 → sort
               ================================ */

            // sort reads from pipe2
            if (dup2(pipe2[0], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }

            // Close unused ends
            close(pipe2[1]);  
            close(pipe2[0]);
            close(pipefd[0]);
            close(pipefd[1]);

            char *sort_args[] = {"sort", NULL};
            execvp("sort", sort_args);
            perror("execvp sort");
            exit(1);
        }
        else {
            /* ================================
               CHILD → P2 → grep
               ================================ */

            // grep reads from pipefd and writes to pipe2
            if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            if (dup2(pipe2[1], STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }

            // Close unused ends
            close(pipefd[1]);
            close(pipefd[0]);
            close(pipe2[0]);
            close(pipe2[1]);

            char *grep_args[] = {"grep", argv[1], NULL};
            execvp("grep", grep_args);
            perror("execvp grep");
            exit(1);
        }
    }
    else {
        /* ================================
           PARENT → P1 → cat scores
           ================================ */

        // cat writes into pipefd
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Close unused ends
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        char *cat_args[] = {"cat", "scores", NULL};
        execvp("cat", cat_args);
        perror("execvp cat");
        exit(1);
    }

    // Should not reach here, but safe cleanup:
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
