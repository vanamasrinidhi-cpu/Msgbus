#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#include "msgbus.h"
#include "../ipc_signals/signal_manager.h"
MessageBus *bus = NULL;
int shm_fd = -1;

void cleanup(int sig) {

    (void)sig;

    if (bus != NULL) {
        bus->initialized = 0;

        munmap(bus, sizeof(MessageBus));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    shm_unlink(SHM_NAME);

    printf("\n[Broker] Shared memory removed.\n");
    printf("[Broker] MsgBus stopped.\n");

    exit(0);
}

int main() {

    if (register_signal(SIGINT, cleanup) == -1) {
    return 1;
}

    shm_unlink(SHM_NAME);

    shm_fd = shm_open(
        SHM_NAME,
        O_CREAT | O_RDWR,
        0666
    );

    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, sizeof(MessageBus)) == -1) {
        perror("ftruncate");
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
        return 1;
    }

    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(
        &mutex_attr,
        PTHREAD_PROCESS_SHARED
    );

    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(
        &cond_attr,
        PTHREAD_PROCESS_SHARED
    );

    pthread_mutex_init(
        &bus->mutex,
        &mutex_attr
    );

    pthread_cond_init(
        &bus->message_available,
        &cond_attr
    );

    bus->initialized = 1;
    bus->next_sequence = 1;

    for (int i = 0; i < MAX_MESSAGES; i++) {
        bus->messages[i].sequence = 0;
    }

    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        bus->subscribers[i].active = 0;
    }

    pthread_mutexattr_destroy(&mutex_attr);
    pthread_condattr_destroy(&cond_attr);

    printf("====================================\n");
    printf("        MsgBus Shared Broker        \n");
    printf("====================================\n");
    printf("Shared memory: %s\n", SHM_NAME);
    printf("Broker PID: %d\n", getpid());
    printf("Maximum messages: %d\n", MAX_MESSAGES);
    printf("Maximum subscribers: %d\n", MAX_SUBSCRIBERS);
    printf("Broker is ready.\n");
    printf("Press Ctrl+C to stop.\n\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
