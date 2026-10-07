#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LOG_FILE "msgbus.log"
#define BUFFER_SIZE 512

void write_log() {
    char message[BUFFER_SIZE];

    printf("Enter message to log: ");
    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1) {
        perror("open");
        return;
    }

    write(fd, message, strlen(message));
    write(fd, "\n", 1);

    close(fd);

    printf("Message written to log successfully.\n");
}

void read_log() {
    char buffer[BUFFER_SIZE];

    int fd = open(LOG_FILE, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return;
    }

    int bytes = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes < 0) {
        perror("read");
        close(fd);
        return;
    }

    buffer[bytes] = '\0';

    printf("\n===== MsgBus Log =====\n");
    printf("%s", buffer);
    printf("======================\n");

    close(fd);
}

int main() {
    char command[50];

    printf("===== MsgBus File I/O =====\n");

    while (1) {
        printf("\nfile> ");

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "write") == 0) {
            write_log();
        }
        else if (strcmp(command, "read") == 0) {
            read_log();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("File I/O stopped.\n");
            break;
        }
        else {
            printf("Commands: write | read | exit\n");
        }
    }

    return 0;
}
