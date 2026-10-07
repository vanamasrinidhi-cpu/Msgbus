#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "process_manager.h"

void create_publisher() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        printf("[Publisher] Child process created.\n");
        printf("[Publisher] PID: %d\n", getpid());
        printf("[Publisher] Parent PID: %d\n", getppid());

        execlp("sleep", "sleep", "2", NULL);

        perror("exec");
        exit(1);
    }

    waitpid(pid, NULL, 0);
    printf("[Broker] Publisher process completed.\n");
}

void create_subscriber() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        printf("[Subscriber] Child process created.\n");
        printf("[Subscriber] PID: %d\n", getpid());
        printf("[Subscriber] Parent PID: %d\n", getppid());

        execlp("sleep", "sleep", "2", NULL);

        perror("exec");
        exit(1);
    }

    waitpid(pid, NULL, 0);
    printf("[Broker] Subscriber process completed.\n");
}

void show_process_info() {
    printf("\n[Broker] Current Process Information\n");
    printf("PID       : %d\n", getpid());
    printf("Parent PID: %d\n", getppid());
}
