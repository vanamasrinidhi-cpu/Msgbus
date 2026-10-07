#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <sys/types.h>

pid_t start_process(const char *program);

pid_t start_process_with_input(
    const char *program,
    const char *input
);

void stop_process(
    pid_t *pid,
    const char *name
);

int process_running(pid_t pid);

#endif
