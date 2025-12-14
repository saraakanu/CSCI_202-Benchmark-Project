#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define SEM_NAME "/bank_sem"

// Function prototypes
void DadProcess(int *bank, sem_t *mutex);
void MomProcess(int *bank, sem_t *mutex);
void StudentProcess(int *bank, sem_t *mutex);

// Global flag for termination
volatile sig_atomic_t keep_running = 1;
void handle_sigint(int sig) { keep_running = 0; }

int main(int argc, char *argv[]) {
    int num_parents = 1; // Dad only by default
    int num_children = 1; // 1 student by default

    if (argc >= 3) {
        num_parents = atoi(argv[1]);   // 1 or 2 (Dad/Mom)
        num_children = atoi(argv[2]);  // number of students
    }

    // Set up signal handler
    signal(SIGINT, handle_sigint);

    // Shared memory for BankAccount
    int ShmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) { perror("shmget"); exit(1); }

    int *BankAccount = (int *) shmat(ShmID, NULL, 0);
    *BankAccount = 0;

    // Semaphore
    sem_t *mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED) { perror("sem_open"); exit(1); }

    pid_t pid;
    srand(time(NULL));

    // Fork Parents
    for (int i = 0; i < num_parents; i++) {
        pid = fork();
        if (pid == 0) {
            srand(time(NULL) ^ getpid());
            if (num_parents == 2 && i == 1) {
                // Mom
                while (keep_running) {
                    sleep(rand() % 11); // 0-10s
                    sem_wait(mutex);
                    MomProcess(BankAccount, mutex);
                    sem_post(mutex);
                }
            } else {
                // Dad
                while (keep_running) {
                    sleep(rand() % 6); // 0-5s
                    sem_wait(mutex);
                    DadProcess(BankAccount, mutex);
                    sem_post(mutex);
                }
            }
            shmdt(BankAccount);
            exit(0);
        }
    }

    // Fork Children
    for (int i = 0; i < num_children; i++) {
        pid = fork();
        if (pid == 0) {
            srand(time(NULL) ^ getpid());
            while (keep_running) {
                sleep(rand() % 6); // 0-5s
                sem_wait(mutex);
                StudentProcess(BankAccount, mutex);
                sem_post(mutex);
            }
            shmdt(BankAccount);
            exit(0);
        }
    }

    // Parent waits for CTRL-C
    while (keep_running) sleep(1);

    // Cleanup: Kill all children
    kill(0, SIGTERM);
    wait(NULL);

    sem_close(mutex);
    sem_unlink(SEM_NAME);
    shmdt(BankAccount);
    shmctl(ShmID, IPC_RMID, NULL);

    printf("\nBank simulation terminated.\n");
    return 0;
}

// Dad Logic
void DadProcess(int *bank, sem_t *mutex) {
    int localBalance = *bank;
    printf("Dear Old Dad: Attempting to Check Balance\n");
    int random_num = rand() % 2; // 0 even, 1 odd

    if (random_num == 0) { // Attempt deposit
        if (localBalance < 100) {
            int amount = rand() % 101; // 0-100
            if (amount % 2 == 0) {
                localBalance += amount;
                printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
            } else {
                printf("Dear Old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
        }
    } else {
        printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
    }

    *bank = localBalance;
}

// Mom Logic
void MomProcess(int *bank, sem_t *mutex) {
    int localBalance = *bank;
    printf("Lovable Mom: Attempting to Check Balance\n");

    if (localBalance <= 100) {
        int amount = rand() % 126; // 0-125
        localBalance += amount;
        printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, localBalance);
    } else {
        printf("Lovable Mom: Last Checking Balance = $%d\n", localBalance);
    }

    *bank = localBalance;
}

// Student Logic
void StudentProcess(int *bank, sem_t *mutex) {
    int localBalance = *bank;
    printf("Poor Student: Attempting to Check Balance\n");

    int random_num = rand() % 2;
    if (random_num == 0) { // Attempt withdrawal
        int need = rand() % 50 + 1; // 1-50
        printf("Poor Student needs $%d\n", need);
        if (need <= localBalance) {
            localBalance -= need;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
        }
    } else {
        printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
    }

    *bank = localBalance;
}
