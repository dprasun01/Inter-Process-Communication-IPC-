//Author: Siyam Al Shahriar

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

void ParentProcess(int SharedMem[]);
void ChildProcess(int SharedMem[]);

int main() {
    int ShmID;
    int *ShmPTR;

    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of two integers...\n");

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (ShmPTR == (void *) -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    ShmPTR[0] = 0;  // BankAccount
    ShmPTR[1] = 0;  // Turn

    pid_t pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        ChildProcess(ShmPTR);
        exit(0);
    } else {
        ParentProcess(ShmPTR);
    }

    wait(NULL);
    printf("Server has detected the completion of its child...\n");

    shmdt((void *) ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void ParentProcess(int SharedMem[]) {
    int account, balance;
    srand(time(NULL));

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6);
        account = SharedMem[0];
        while (SharedMem[1] != 0);  // Busy wait

        if (account <= 100) {
            balance = rand() % 101;
            if (balance % 2 == 0) {
                account += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 1;  // Set Turn to 1
    }
}

void ChildProcess(int SharedMem[]) {
    int account, balance;
    srand(time(NULL) ^ (getpid() << 16));

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6);
        account = SharedMem[0];
        while (SharedMem[1] != 1);  // Busy wait

        balance = rand() % 51;
        printf("Poor Student needs $%d\n", balance);
        if (balance <= account) {
            account -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 0;  // Set Turn to 0
    }
}