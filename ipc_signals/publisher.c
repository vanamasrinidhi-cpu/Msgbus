#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME "msgbus_fifo"

int main() {
    char message[256];

    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    int fd = open(FIFO_NAME, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, message, strlen(message));

    printf("Message published successfully.\n");

    close(fd);

    return 0;
}
