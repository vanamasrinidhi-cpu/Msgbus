#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define FIFO_NAME "msgbus_fifo"
#define MESSAGE_SIZE 256

volatile sig_atomic_t message_signal = 0;

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        message_signal = 1;
    }
}

int main() {
    char message[MESSAGE_SIZE];

    signal(SIGUSR1, handle_signal);

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
    }

    printf("===== MsgBus Broker =====\n");
    printf("Broker PID: %d\n", getpid());
    printf("Waiting for messages...\n");

    int fd = open(FIFO_NAME, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        memset(message, 0, MESSAGE_SIZE);

        int bytes = read(fd, message, MESSAGE_SIZE - 1);

        if (bytes > 0) {
            message[bytes] = '\0';

            printf("\n[Broker] Message received: %s\n", message);

            message_signal = 0;
        }
    }

    close(fd);
    unlink(FIFO_NAME);

    return 0;
}
