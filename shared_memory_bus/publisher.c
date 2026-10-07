#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include "msgbus.h"

#define LOG_FILE "msgbus.log"

int main() {

    char topic[TOPIC_SIZE];
    char message[MESSAGE_SIZE];

    int shm_fd = shm_open(
        SHM_NAME,
        O_RDWR,
        0666
    );

    if (shm_fd == -1) {
        perror("shm_open");
        printf("Start the broker first.\n");
        return 1;
    }

    MessageBus *bus = mmap(
        NULL,
        sizeof(MessageBus),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    if (bus == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }

    printf("===== MsgBus Publisher =====\n");

    printf("Enter topic: ");
    fgets(topic, sizeof(topic), stdin);
    topic[strcspn(topic, "\n")] = '\0';

    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    pthread_mutex_lock(&bus->mutex);

    unsigned long sequence = bus->next_sequence;

    int index = sequence % MAX_MESSAGES;

    bus->messages[index].sequence = sequence;

    strncpy(
        bus->messages[index].topic,
        topic,
        TOPIC_SIZE - 1
    );

    bus->messages[index].topic[TOPIC_SIZE - 1] = '\0';

    strncpy(
        bus->messages[index].message,
        message,
        MESSAGE_SIZE - 1
    );

    bus->messages[index].message[MESSAGE_SIZE - 1] = '\0';

    bus->next_sequence++;

    /* Write message to log file */
    int log_fd = open(
        LOG_FILE,
        O_WRONLY | O_CREAT | O_APPEND,
        0644
    );

    if (log_fd != -1) {

        char log_entry[400];

        int length = snprintf(
            log_entry,
            sizeof(log_entry),
            "Sequence: %lu | Topic: %s | Message: %s\n",
            sequence,
            topic,
            message
        );

        write(log_fd, log_entry, length);

        close(log_fd);
    }

    printf("\n[Publisher] Message published.\n");
    printf("[Publisher] Sequence: %lu\n", sequence);
    printf("[Publisher] Topic   : %s\n", topic);
    printf("[Publisher] Message : %s\n", message);
    printf("[Publisher] Message logged successfully.\n");

    pthread_cond_broadcast(
        &bus->message_available
    );

    pthread_mutex_unlock(&bus->mutex);

    munmap(bus, sizeof(MessageBus));
    close(shm_fd);

    return 0;
}
