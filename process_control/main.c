#include <stdio.h>
#include <string.h>
#include "process_manager.h"

int main() {
    char command[50];

    printf("===== MsgBus Process Control =====\n");

    while (1) {
        printf("\nprocess> ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "publisher") == 0) {
            create_publisher();
        }
        else if (strcmp(command, "subscriber") == 0) {
            create_subscriber();
        }
        else if (strcmp(command, "info") == 0) {
            show_process_info();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("Process control stopped.\n");
            break;
        }
        else {
            printf("Commands: publisher | subscriber | info | exit\n");
        }
    }

    return 0;
}
