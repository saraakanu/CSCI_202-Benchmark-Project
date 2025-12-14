#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_LOOPS 25

int main() {
    int ShmID;
    int *ShmPTR;
    pid_t pid;

    srand(time(NULL));

    // Create shared memory for TWO integers: BankAccount, Turn
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error ***\n");
        exit(1);
    }

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if ((int)ShmPTR == -1) {
        printf("*** shmat error ***\n");
        exit(1);
    }

    // Initialize shared variables
    ShmPTR[0] = 0;   // BankAccount
    ShmPTR[1] = 0;   // Turn = 0 (father goes first)

    printf("Shared memory set up. Starting processes…\n");

    pid = fork();

    if (pid < 0) {
        printf("*** fork error ***\n");
        exit(1);
    }

    else if (pid == 0) {
        //--------------------------------------
        // CHILD PROCESS (Poor Student)
        //--------------------------------------
        int account, need, i;

        for (i = 0; i < NUM_LOOPS; i++) {
            sleep(rand() % 6);

            // Wait for Turn == 1
            while (ShmPTR[1] != 1)
                ; // busy wait

            account = ShmPTR[0];

            need = rand() % 50;
            printf("Poor Student needs $%d\n", need);

            if (need <= account) {
                account -= need;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n",
                       need, account);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", account);
            }

            ShmPTR[0] = account;   // write back updated balance
            ShmPTR[1] = 0;         // give turn back to Dad
        }

        exit(0);
    }

    else {
        //--------------------------------------
        // PARENT PROCESS (Dear Old Dad)
        //--------------------------------------
        int account, give, i;

        for (i = 0; i < NUM_LOOPS; i++) {
            sleep(rand() % 6);

            // Wait for Turn == 0
            while (ShmPTR[1] != 0)
                ; // busy wait

            account = ShmPTR[0];

            if (account <= 100) {
                give = rand() % 100;

                if (give % 2 == 0) {
                    account += give;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n",
                           give, account);
                } else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                       account);
            }

            ShmPTR[0] = account; // write back updated balance
            ShmPTR[1] = 1;       // give turn to Student
        }

        wait(NULL);
        shmdt((void *)ShmPTR);
        shmctl(ShmID, IPC_RMID, NULL);

        printf("Parent finished and cleaned up shared memory.\n");
        exit(0);
    }
}
