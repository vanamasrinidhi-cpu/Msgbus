#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "signal_manager.h"

int register_signal(int signal_number, void (*handler)(int)) {

    struct sigaction action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(signal_number, &action, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    return 0;
}


int send_signal(pid_t pid, int signal_number) {

    if (kill(pid, signal_number) == -1) {

        if (errno == ESRCH) {
            printf(
                "[Signal Manager] Process %d not found.\n",
                pid
            );
        } else {
            perror("kill");
        }

        return -1;
    }

    return 0;
}
