#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("===== MsgBus Memory Management =====\n");

    int *message_buffer = malloc(5 * sizeof(int));

    if (message_buffer == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        message_buffer[i] = i + 1;
    }

    printf("\n[Parent] Allocated message buffer.\n");
    printf("[Parent] Buffer address: %p\n", (void *)message_buffer);
    printf("[Parent] First value: %d\n", message_buffer[0]);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        free(message_buffer);
        return 1;
    }

    if (pid == 0) {
        printf("\n[Child] Process created using fork().\n");
        printf("[Child] Buffer address: %p\n", (void *)message_buffer);
        printf("[Child] Before modification: %d\n", message_buffer[0]);

        message_buffer[0] = 100;

        printf("[Child] After modification: %d\n", message_buffer[0]);

        free(message_buffer);
        exit(0);
    }

    wait(NULL);

    printf("\n[Parent] Child completed.\n");
    printf("[Parent] Parent value remains: %d\n", message_buffer[0]);

    free(message_buffer);

    printf("[Parent] Memory released successfully.\n");

    return 0;
}
