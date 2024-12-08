#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void ChildProcess(int []);
void DepositMoney(int []);

int main(int argc, char *argv[]) {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Shared variables: BankAccount and Turn (0 for parent, 1 for child)
    int BankAccount = 0;
    int Turn = 0;

    if (argc != 1) {
        printf("Error\n");
        exit(1);
    }

    // Create shared memory
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }

    // Attach shared memory
    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Process has received shared memory of two integers...\n");

    // Initialize shared memory with initial values
    ShmPTR[0] = BankAccount;
    ShmPTR[1] = Turn;

    printf("Process has attached the shared memory...\n");
    printf("Original Bank Account = %d\n", BankAccount);

    // Fork a child process
    pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        // Child process: simulate withdrawal 25 times
        for (int i = 0; i < 25; i++) {
            ChildProcess(ShmPTR);
        }
        exit(0);  // Exit child process after work is done
    }

    // Parent process: simulate deposit 25 times
    for (int i = 0; i < 25; i++) {
        int r = rand() % 5;
        sleep(r);  // Sleep for a random time

        // Wait for parent's turn (SharedMem[1] == 0 means it's the parent's turn to deposit)
        while (ShmPTR[1] != 0) {
            // Busy wait until it's parent's turn
        }

        int account = ShmPTR[0];

        if (account <= 100) {
            DepositMoney(ShmPTR);  // Deposit money if account is less than or equal to 100
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        // Set turn to 1 to allow child to work
        ShmPTR[1] = 1;
    }

    // Wait for the child process to complete
    wait(&status);
    printf("Process has detected the completion of its child...\n");

    // Detach and remove shared memory
    shmdt((void *)ShmPTR);
    printf("Process has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Process has removed its shared memory...\n");

    printf("Process exits...\n");
    exit(0);
}

// Child process function to simulate withdrawal
void ChildProcess(int SharedMem[]) {
    int r = rand() % 20;
    sleep(r);  // Sleep for a random amount of time

    int account = SharedMem[0];

    // Wait for the child's turn (SharedMem[1] == 1 means it's child's turn to withdraw)
    while (SharedMem[1] != 1) {
        // Busy wait until it's child's turn
    }

    int balance = rand() % 50;
    printf("Poor Student needs $%d\n", balance);

    if (balance <= account) {
        account -= balance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
    } else {
        printf("Poor Student: Not Enough Cash ($%d)\n", account);
    }

    // Update shared memory with new balance
    SharedMem[0] = account;
    // Set turn to 0 to allow parent to work
    SharedMem[1] = 0;
}

// Function to simulate deposit
void DepositMoney(int SharedMem[]) {
    int balance = rand() % 100;
    int account = SharedMem[0];

    if (balance % 2 == 0) {
        account += balance;
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
    } else {
        printf("Dear old Dad: Doesn't have any money to give\n");
    }

    // Update shared memory with new balance
    SharedMem[0] = account;
    // Set turn to 1 to allow child to work
    SharedMem[1] = 1;
}