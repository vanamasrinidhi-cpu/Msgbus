#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "msgbus.h"

MessageBus *bus = NULL;
int shm_fd = -1;
int subscriber_id = -1;

void cleanup(int sig) {

    (void)sig;

    if (bus != NULL && subscriber_id != -1) {

        pthread_mutex_lock(&bus->mutex);

        bus->subscribers[subscriber_id].active = 0;

        pthread_mutex_unlock(&bus->mutex);

        munmap(bus, sizeof(MessageBus));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    printf("\n[Subscriber] Unsubscribed successfully.\n");

    exit(0);
}

int main() {

    char topic[TOPIC_SIZE];

    signal(SIGINT, cleanup);

    shm_fd = shm_open(
        SHM_NAME,
        O_RDWR,
        0666
    );

    if (shm_fd == -1) {
        perror("shm_open");
        printf("Start the broker first.\n");
        return 1;
    }

    bus = mmap(
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

    if (!bus->initialized) {
        printf("Broker is not ready.\n");
        munmap(bus, sizeof(MessageBus));
        close(shm_fd);
        return 1;
    }

    printf("===== MsgBus Subscriber =====\n");

    printf("Enter topic to subscribe: ");
    fgets(topic, sizeof(topic), stdin);

    topic[strcspn(topic, "\n")] = '\0';

    pthread_mutex_lock(&bus->mutex);

    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {

        if (!bus->subscribers[i].active) {

            subscriber_id = i;

            bus->subscribers[i].active = 1;
            bus->subscribers[i].pid = getpid();

            strncpy(
                bus->subscribers[i].topic,
                topic,
                TOPIC_SIZE - 1
            );

            bus->subscribers[i].topic[TOPIC_SIZE - 1] = '\0';

            bus->subscribers[i].last_sequence =
                bus->next_sequence - 1;

            break;
        }
    }

    pthread_mutex_unlock(&bus->mutex);

    if (subscriber_id == -1) {

        printf("No subscriber slots available.\n");

        munmap(bus, sizeof(MessageBus));
        close(shm_fd);

        return 1;
    }

    printf(
        "[Subscriber] ID      : %d\n",
        subscriber_id
    );

    printf(
        "[Subscriber] PID     : %d\n",
        getpid()
    );

    printf(
        "[Subscriber] Topic   : %s\n",
        topic
    );

    printf("[Subscriber] Waiting for messages...\n\n");

    while (1) {

        pthread_mutex_lock(&bus->mutex);

        unsigned long latest_sequence =
            bus->next_sequence - 1;

        while (
            bus->subscribers[subscriber_id].last_sequence
            >= latest_sequence
        ) {

            pthread_cond_wait(
                &bus->message_available,
                &bus->mutex
            );

            latest_sequence =
                bus->next_sequence - 1;
        }

        unsigned long last_sequence =
            bus->subscribers[subscriber_id].last_sequence;

        for (
            unsigned long sequence = last_sequence + 1;
            sequence <= latest_sequence;
            sequence++
        ) {

            int index = sequence % MAX_MESSAGES;

            if (
                bus->messages[index].sequence == sequence &&
                strcmp(
                    bus->messages[index].topic,
                    topic
                ) == 0
            ) {

                printf(
                    "[Subscriber] Message received\n"
                );

                printf(
                    "Sequence: %lu\n",
                    sequence
                );

                printf(
                    "Topic   : %s\n",
                    bus->messages[index].topic
                );

                printf(
                    "Message : %s\n\n",
                    bus->messages[index].message
                );
            }
        }

        bus->subscribers[subscriber_id].last_sequence =
            latest_sequence;

        pthread_mutex_unlock(&bus->mutex);
    }

    return 0;
}