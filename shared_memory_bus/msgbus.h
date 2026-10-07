#ifndef MSGBUS_H
#define MSGBUS_H

#include <pthread.h>
#include <sys/types.h>

#define SHM_NAME "/msgbus_shared_memory"

#define MAX_MESSAGES 20
#define MAX_SUBSCRIBERS 10

#define TOPIC_SIZE 50
#define MESSAGE_SIZE 256

typedef struct {
    unsigned long sequence;
    char topic[TOPIC_SIZE];
    char message[MESSAGE_SIZE];
} Message;

typedef struct {
    int active;
    pid_t pid;
    char topic[TOPIC_SIZE];
    unsigned long last_sequence;
} Subscriber;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t message_available;

    int initialized;
    unsigned long next_sequence;

    Message messages[MAX_MESSAGES];
    Subscriber subscribers[MAX_SUBSCRIBERS];

} MessageBus;

#endif
#ifndef MSGBUS_H
#define MSGBUS_H

#include <pthread.h>
#include <sys/types.h>

#define SHM_NAME "/msgbus_shared_memory"

#define MAX_MESSAGES 20
#define MAX_SUBSCRIBERS 10

#define TOPIC_SIZE 50
#define MESSAGE_SIZE 256

typedef struct {
    unsigned long sequence;
    char topic[TOPIC_SIZE];
    char message[MESSAGE_SIZE];
} Message;

typedef struct {
    int active;
    pid_t pid;
    char topic[TOPIC_SIZE];
    unsigned long last_sequence;
} Subscriber;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t message_available;

    int initialized;
    unsigned long next_sequence;

    Message messages[MAX_MESSAGES];
    Subscriber subscribers[MAX_SUBSCRIBERS];

} MessageBus;

#endif
