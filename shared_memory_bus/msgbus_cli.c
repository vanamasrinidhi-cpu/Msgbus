#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "msgbus.h"

#define LOG_FILE "msgbus.log"

pid_t broker_pid = -1;

pid_t subscriber_pids[MAX_SUBSCRIBERS];
int subscriber_count = 0;


/* Check and remove finished subscribers */
void update_subscribers() {

    for (int i = 0; i < subscriber_count; i++) {

        if (subscriber_pids[i] > 0) {

            int status;

            if (waitpid(
                    subscriber_pids[i],
                    &status,
                    WNOHANG
                ) > 0) {

                subscriber_pids[i] = -1;
            }
        }
    }
}


/* Start Broker */
void start_broker() {

    if (broker_pid > 0) {
        printf("\n[MsgBus] Broker is already running.\n");
        return;
    }

    broker_pid = fork();

    if (broker_pid < 0) {
        perror("fork");
        return;
    }

    if (broker_pid == 0) {

        execl(
            "./broker",
            "./broker",
            NULL
        );

        perror("exec");
        exit(1);
    }

    sleep(1);

    printf("\n[MsgBus] Broker started successfully.\n");
    printf("[MsgBus] Broker PID: %d\n", broker_pid);
}


/* Publish Message */
void publish_message() {

    char topic[TOPIC_SIZE];
    char message[MESSAGE_SIZE];

    if (broker_pid <= 0) {
        printf("\n[Error] Start MsgBus first.\n");
        return;
    }

    printf("\n===== Publish Message =====\n");

    printf("Enter topic: ");
    fgets(topic, sizeof(topic), stdin);
    topic[strcspn(topic, "\n")] = '\0';

    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';


    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return;
    }


    pid_t publisher_pid = fork();

    if (publisher_pid < 0) {

        perror("fork");

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        return;
    }


    if (publisher_pid == 0) {

        close(pipe_fd[1]);

        dup2(
            pipe_fd[0],
            STDIN_FILENO
        );

        close(pipe_fd[0]);

        execl(
            "./publisher",
            "./publisher",
            NULL
        );

        perror("exec");
        exit(1);
    }


    close(pipe_fd[0]);

    dprintf(
        pipe_fd[1],
        "%s\n%s\n",
        topic,
        message
    );

    close(pipe_fd[1]);

    waitpid(
        publisher_pid,
        NULL,
        0
    );

    printf(
        "[MsgBus] Message published successfully.\n"
    );
}


/* Subscribe to Topic */
void subscribe_topic() {

    char topic[TOPIC_SIZE];

    if (broker_pid <= 0) {
        printf("\n[Error] Start MsgBus first.\n");
        return;
    }

    update_subscribers();


    if (subscriber_count >= MAX_SUBSCRIBERS) {

        printf(
            "\n[Error] Maximum subscribers reached (%d).\n",
            MAX_SUBSCRIBERS
        );

        return;
    }


    printf("\n===== Subscribe to Topic =====\n");

    printf("Enter topic: ");

    fgets(
        topic,
        sizeof(topic),
        stdin
    );

    topic[strcspn(topic, "\n")] = '\0';


    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return;
    }


    pid_t subscriber_pid = fork();

    if (subscriber_pid < 0) {

        perror("fork");

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        return;
    }


    if (subscriber_pid == 0) {

        close(pipe_fd[1]);

        dup2(
            pipe_fd[0],
            STDIN_FILENO
        );

        close(pipe_fd[0]);

        execl(
            "./subscriber",
            "./subscriber",
            NULL
        );

        perror("exec");
        exit(1);
    }


    close(pipe_fd[0]);


    dprintf(
        pipe_fd[1],
        "%s\n",
        topic
    );

    close(pipe_fd[1]);


    subscriber_pids[subscriber_count] =
        subscriber_pid;

    subscriber_count++;


    printf(
        "\n[MsgBus] Subscriber started.\n"
    );

    printf(
        "[MsgBus] Subscriber PID: %d\n",
        subscriber_pid
    );

    printf(
        "[MsgBus] Subscribed to topic: %s\n",
        topic
    );

    printf(
        "[MsgBus] Active subscribers: %d/%d\n",
        subscriber_count,
        MAX_SUBSCRIBERS
    );
}


/* Show Status */
void show_status() {

    update_subscribers();

    printf("\n");

    printf(
        "+--------------------------------------+\n"
    );

    printf(
        "|            MSGBUS STATUS             |\n"
    );

    printf(
        "+--------------------------------------+\n"
    );


    if (broker_pid > 0)

        printf(
            "| Broker      : RUNNING  PID %-8d |\n",
            broker_pid
        );

    else

        printf(
            "| Broker      : STOPPED               |\n"
        );


    int active = 0;

    for (int i = 0; i < subscriber_count; i++) {

        if (subscriber_pids[i] > 0) {

            printf(
                "| Subscriber %d: RUNNING  PID %-8d |\n",
                i + 1,
                subscriber_pids[i]
            );

            active++;
        }
    }


    if (active == 0) {

        printf(
            "| Subscribers : NONE                   |\n"
        );
    }


    printf(
        "| Active      : %d/%d subscribers       |\n",
        active,
        MAX_SUBSCRIBERS
    );


    printf(
        "+--------------------------------------+\n"
    );
}


/* View Log */
void view_log() {

    FILE *file = fopen(
        LOG_FILE,
        "r"
    );

    if (file == NULL) {

        printf(
            "\n[MsgBus] No messages logged yet.\n"
        );

        return;
    }


    char line[500];

    printf("\n");

    printf(
        "+--------------------------------------+\n"
    );

    printf(
        "|             MESSAGE LOG              |\n"
    );

    printf(
        "+--------------------------------------+\n"
    );


    while (
        fgets(
            line,
            sizeof(line),
            file
        )
    ) {

        printf(
            "%s",
            line
        );
    }


    printf(
        "+--------------------------------------+\n"
    );


    fclose(file);
}


/* Stop All Subscribers */
void stop_subscribers() {

    update_subscribers();

    for (int i = 0; i < subscriber_count; i++) {

        if (subscriber_pids[i] > 0) {

            kill(
                subscriber_pids[i],
                SIGINT
            );

            waitpid(
                subscriber_pids[i],
                NULL,
                0
            );

            printf(
                "[MsgBus] Subscriber %d stopped.\n",
                i + 1
            );

            subscriber_pids[i] = -1;
        }
    }

    subscriber_count = 0;
}


/* Stop Broker */
void stop_broker() {

    if (broker_pid > 0) {

        kill(
            broker_pid,
            SIGINT
        );

        waitpid(
            broker_pid,
            NULL,
            0
        );

        printf(
            "[MsgBus] Broker stopped.\n"
        );

        broker_pid = -1;
    }
}


/* Stop MsgBus */
void stop_msgbus() {

    printf(
        "\n[MsgBus] Stopping MsgBus...\n"
    );

    stop_subscribers();

    stop_broker();

    printf(
        "[MsgBus] All processes stopped.\n"
    );
}


/* Display Menu */
void show_menu() {

    printf("\n");

    printf(
        "+----------------------------------------+\n"
    );

    printf(
        "|              MSGBUS SYSTEM             |\n"
    );

    printf(
        "|        Shared-Memory Message Bus       |\n"
    );

    printf(
        "+----------------------------------------+\n"
    );

    printf(
        "|                                        |\n"
    );

    printf(
        "|   [1]  >  Start Message Bus            |\n"
    );

    printf(
        "|   [2]  >  Publish Message              |\n"
    );

    printf(
        "|   [3]  >  Subscribe to Topic           |\n"
    );

    printf(
        "|   [4]  >  System Status                |\n"
    );

    printf(
        "|   [5]  >  View Message Log             |\n"
    );

    printf(
        "|   [6]  >  Stop Message Bus             |\n"
    );

    printf(
        "|   [7]  >  Exit                         |\n"
    );

    printf(
        "|                                        |\n"
    );

    printf(
        "+----------------------------------------+\n"
    );
}


/* Main */
int main() {

    int choice;


    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {

        subscriber_pids[i] = -1;
    }


    printf("\n");

    printf(
        "+----------------------------------------+\n"
    );

    printf(
        "|                                        |\n"
    );

    printf(
        "|              MSGBUS                    |\n"
    );

    printf(
        "|                                        |\n"
    );

    printf(
        "|      Shared-Memory Message Bus         |\n"
    );

    printf(
        "|                                        |\n"
    );

    printf(
        "+----------------------------------------+\n"
    );


    printf(
        "\nInterface PID: %d\n",
        getpid()
    );


    while (1) {

        show_menu();

        printf(
            "\nEnter your choice [1-7]: "
        );


        if (
            scanf(
                "%d",
                &choice
            ) != 1
        ) {

            while (
                getchar() != '\n'
            );

            printf(
                "\n[Error] Enter a number from 1 to 7.\n"
            );

            continue;
        }


        while (
            getchar() != '\n'
        );


        switch (choice) {

            case 1:

                start_broker();

                break;


            case 2:

                publish_message();

                break;


            case 3:

                subscribe_topic();

                break;


            case 4:

                show_status();

                break;


            case 5:

                view_log();

                break;


            case 6:

                stop_msgbus();

                break;


            case 7:

                printf(
                    "\n[MsgBus] Exiting...\n"
                );

                stop_msgbus();

                printf(
                    "[MsgBus] Interface stopped.\n"
                );

                return 0;


            default:

                printf(
                    "\n[Error] Invalid choice. "
                    "Please select 1-7.\n"
                );
        }
    }


    return 0;
}
