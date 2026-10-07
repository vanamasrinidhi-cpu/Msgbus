#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"

int main() {
    char command[100];

    printf("================================\n");
    printf("        MsgBus Message Bus       \n");
    printf("================================\n");

    printf("MsgBus Process ID: %d\n", getpid());
    printf("Type 'help' to see commands.\n\n");

    while (1) {
        printf("msgbus> ");

        if (fgets(command, sizeof(command), stdin) == NULL)
            break;

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "help") == 0) {
            show_help();
        }
        else if (strcmp(command, "start") == 0) {
            start_bus();
        }
        else if (strcmp(command, "status") == 0) {
            show_status();
        }
        else if (strcmp(command, "pid") == 0) {
            show_pid();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("MsgBus stopped.\n");
            break;
        }
        else if (strlen(command) == 0) {
            continue;
        }
        else {
            printf("Unknown command. Type 'help'.\n");
        }
    }

    return 0;
}
