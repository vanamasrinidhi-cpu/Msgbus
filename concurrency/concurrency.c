#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define MESSAGE_SIZE 100
#define MESSAGE_COUNT 6

char message_queue[QUEUE_SIZE][MESSAGE_SIZE];

int front = 0;
int rear = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;

sem_t message_count;

void *publisher(void *arg) {
    for (int i = 1; i <= MESSAGE_COUNT; i++) {

        pthread_mutex_lock(&queue_mutex);

        while ((rear + 1) % QUEUE_SIZE == front) {
            pthread_cond_wait(&queue_not_full, &queue_mutex);
        }

        snprintf(message_queue[rear],
                 MESSAGE_SIZE,
                 "Message %d from Publisher",
                 i);

        printf("[Publisher] Published: %s\n", message_queue[rear]);

        rear = (rear + 1) % QUEUE_SIZE;

        pthread_mutex_unlock(&queue_mutex);

        sem_post(&message_count);

        pthread_cond_signal(&queue_not_empty);

        sleep(1);
    }

    return NULL;
}

void *subscriber(void *arg) {
    for (int i = 1; i <= MESSAGE_COUNT; i++) {

        sem_wait(&message_count);

        pthread_mutex_lock(&queue_mutex);

        while (front == rear) {
            pthread_cond_wait(&queue_not_empty, &queue_mutex);
        }

        printf("[Subscriber] Received: %s\n",
               message_queue[front]);

        front = (front + 1) % QUEUE_SIZE;

        pthread_mutex_unlock(&queue_mutex);

        pthread_cond_signal(&queue_not_full);
    }

    return NULL;
}

int main() {
    pthread_t publisher_thread;
    pthread_t subscriber_thread;

    printf("====================================\n");
    printf("     MsgBus Concurrency System      \n");
    printf("====================================\n\n");

    sem_init(&message_count, 0, 0);

    printf("[Broker] Starting publisher and subscriber...\n\n");

    pthread_create(&publisher_thread,
                   NULL,
                   publisher,
                   NULL);

    pthread_create(&subscriber_thread,
                   NULL,
                   subscriber,
                   NULL);

    pthread_join(publisher_thread, NULL);
    pthread_join(subscriber_thread, NULL);

    sem_destroy(&message_count);

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_not_empty);
    pthread_cond_destroy(&queue_not_full);

    printf("\n[Broker] All messages processed successfully.\n");
    printf("[Broker] Concurrency test completed.\n");

    return 0;
}
