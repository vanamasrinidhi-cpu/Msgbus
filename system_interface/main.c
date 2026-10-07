#include <stdio.h>
#include "interface.h"

void show_main_menu() {

    printf("\n");
    printf("+----------------------------------------+\n");
    printf("|              MSGBUS SYSTEM             |\n");
    printf("|        Operating System Message Bus    |\n");
    printf("+----------------------------------------+\n");
    printf("|                                        |\n");
    printf("|   [1]  Start Message Bus               |\n");
    printf("|   [2]  Publish Message                 |\n");
    printf("|   [3]  Subscribe to Topic              |\n");
    printf("|   [4]  System Status                   |\n");
    printf("|   [5]  View Message Log                |\n");
    printf("|   [6]  Stop Message Bus                |\n");
    printf("|   [7]  Exit                            |\n");
    printf("|                                        |\n");
    printf("+----------------------------------------+\n");
}

int get_choice() {

    int choice;

    printf("\nEnter your choice [1-7]: ");

    if (scanf("%d", &choice) != 1) {

        while (getchar() != '\n');

        return -1;
    }

    while (getchar() != '\n');

    return choice;
}
