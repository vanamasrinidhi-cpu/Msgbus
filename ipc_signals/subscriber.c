#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    printf("===== MsgBus Subscriber =====\n");
    printf("Subscriber PID: %d\n", getpid());

    printf("Subscriber is ready to receive messages.\n");

    while (1) {
        pause();
    }

    return 0;
}
