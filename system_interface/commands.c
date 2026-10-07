#include <stdio.h>
#include <unistd.h>
#include "commands.h"

static int bus_running = 0;

void show_help() {
    printf("\nAvailable commands:\n");
    printf("  start   - Start the message bus\n");
    printf("  status  - Show bus status\n");
    printf("  pid     - Show process ID\n");
    printf("  help    - Show available commands\n");
    printf("  exit    - Stop MsgBus\n\n");
}

void start_bus() {
    if (bus_running) {
        printf("MsgBus is already running.\n");
    } else {
        bus_running = 1;
        printf("Message Bus started.\n");
    }
}

void show_status() {
    if (bus_running)
        printf("MsgBus status: RUNNING\n");
    else
        printf("MsgBus status: STOPPED\n");
}

void show_pid() {
    printf("MsgBus Process ID: %d\n", getpid());
}
