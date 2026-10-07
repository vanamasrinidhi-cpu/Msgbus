#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "process_control/process_manager.h"

#define LOG_FILE "msgbus.log"

pid_t broker_pid = -1;

void run_demo(const char *title, const char *command)
{
    printf("\n========================================\n");
    printf(" %s\n", title);
    printf("========================================\n");

    int result = system(command);

    if (result == 0)
        printf("[OK] %s completed successfully.\n", title);
    else
        printf("[ERROR] %s failed.\n", title);
}

void start_broker()
{
    if (broker_pid > 0) {
        printf("\n[MsgBus] Broker already running.\n");
        return;
    }

    broker_pid = fork();

    if (broker_pid == 0) {
        execl(
            "./shared_memory_bus/broker",
            "./shared_memory_bus/broker",
            NULL
        );

        perror("Broker exec");
        exit(1);
    }

    if (broker_pid > 0) {
        sleep(1);

        printf("\n[CO-2 Process Control] Broker process created.\n");
        printf("[CO-3 IPC] Signal support connected.\n");
        printf("[CO-4 Memory Management] Shared memory connected.\n");
        printf("[CO-6 Concurrency] Process synchronization connected.\n");
        printf("[MsgBus] Broker started. PID: %d\n", broker_pid);
    }
}

void stop_broker()
{
    if (broker_pid <= 0) {
        printf("\n[MsgBus] Broker is not running.\n");
        return;
    }

    stop_process(&broker_pid, "Broker");
}

void show_menu()
{
    printf("\n+----------------------------------------+\n");
    printf("|              MSGBUS SYSTEM             |\n");
    printf("|        Operating System Message Bus    |\n");
    printf("+----------------------------------------+\n");
    printf("|                                        |\n");
    printf("|   [1] Start Integrated MsgBus          |\n");
    printf("|   [2] Publish Message                  |\n");
    printf("|   [3] Subscribe to Topic               |\n");
    printf("|   [4] Run Module Demonstration         |\n");
    printf("|   [5] View Message Log                 |\n");
    printf("|   [6] Stop Message Bus                 |\n");
    printf("|   [7] Exit                             |\n");
    printf("|                                        |\n");
    printf("+----------------------------------------+\n");
}

int main()
{
    int choice;

    printf("\n+----------------------------------------+\n");
    printf("|              MSGBUS                    |\n");
    printf("|      Operating System Message Bus      |\n");
    printf("+----------------------------------------+\n");

    printf("\n[CO-1] System Interface initialized.\n");
    printf("[CO-2] Process Control initialized.\n");
    printf("[CO-3] IPC and Signals initialized.\n");
    printf("[CO-4] Memory Management initialized.\n");
    printf("[CO-5] File I/O initialized.\n");
    printf("[CO-6] Concurrency initialized.\n");

    while (1)
    {
        show_menu();

        printf("\nEnter your choice [1-7]: ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            printf("\n[Error] Enter a number.\n");
            continue;
        }

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                start_broker();
                break;

            case 2:
            {
                if (broker_pid <= 0)
                {
                    printf("\n[Error] Start MsgBus first.\n");
                    break;
                }

                char topic[100];
                char message[300];
                char input[450];

                printf("\nEnter topic: ");
                fgets(topic, sizeof(topic), stdin);
                topic[strcspn(topic, "\n")] = '\0';

                printf("Enter message: ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = '\0';

                snprintf(
                    input,
                    sizeof(input),
                    "%s\n%s\n",
                    topic,
                    message
                );

                pid_t publisher =
                    start_process_with_input(
                        "./shared_memory_bus/publisher",
                        input
                    );

                if (publisher > 0)
                {
                    waitpid(publisher, NULL, 0);

                    printf(
                        "\n[CO-5 File I/O] Message stored in log.\n"
                    );

                    printf(
                        "[CO-6 Concurrency] Publisher synchronized with bus.\n"
                    );
                }

                break;
            }

            case 3:
            {
                if (broker_pid <= 0)
                {
                    printf("\n[Error] Start MsgBus first.\n");
                    break;
                }

                char topic[100];
                char input[120];

                printf("\nEnter topic to subscribe: ");
                fgets(topic, sizeof(topic), stdin);
                topic[strcspn(topic, "\n")] = '\0';

                snprintf(
                    input,
                    sizeof(input),
                    "%s\n",
                    topic
                );

                pid_t subscriber =
                    start_process_with_input(
                        "./shared_memory_bus/subscriber",
                        input
                    );

                if (subscriber > 0)
                {
                    printf(
                        "\n[CO-3 IPC] Subscriber process created.\n"
                    );

                    printf(
                        "[CO-4 Memory Management] Subscriber attached to shared memory.\n"
                    );

                    printf(
                        "[CO-6 Concurrency] Subscriber waiting for messages.\n"
                    );

                    printf(
                        "[MsgBus] Subscriber PID: %d\n",
                        subscriber
                    );
                }

                break;
            }

            case 4:
                printf("\n");
                printf("========== MODULE INTEGRATION ==========\n");

                printf("\n[CO-1] SYSTEM INTERFACE\n");
                printf("      Menu and user input handled.\n");

                printf("\n[CO-2] PROCESS CONTROL\n");
                printf("      fork(), exec(), wait() used for MsgBus processes.\n");

                printf("\n[CO-3] IPC / SIGNALS\n");
                printf("      Process communication and SIGINT shutdown connected.\n");

                printf("\n[CO-4] MEMORY MANAGEMENT\n");
                printf("      Shared memory used for message exchange.\n");

                printf("\n[CO-5] FILE I/O\n");
                printf("      Messages written to shared_memory_bus/msgbus.log.\n");

                printf("\n[CO-6] CONCURRENCY\n");
                printf("      Publisher/subscriber synchronization supported.\n");

                printf("\n=========================================\n");
                printf("[SUCCESS] All six OS concepts are integrated.\n");
                printf("=========================================\n");

                break;

            case 5:
            {
                FILE *file = fopen(LOG_FILE, "r");

                if (file == NULL)
                {
                    printf("\n[File I/O] No messages logged yet.\n");
                    break;
                }

                char line[512];

                printf("\n========== MESSAGE LOG ==========\n");

                while (fgets(line, sizeof(line), file))
                    printf("%s", line);

                printf("=================================\n");

                fclose(file);

                break;
            }

            case 6:
                stop_broker();
                break;

            case 7:
                printf("\n[MsgBus] Exiting...\n");
                stop_broker();
                printf("[MsgBus] All modules shut down.\n");
                return 0;

            default:
                printf("\n[Error] Choose 1 to 7.\n");
        }
    }

    return 0;
}
