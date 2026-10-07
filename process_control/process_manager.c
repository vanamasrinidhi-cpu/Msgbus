
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#include "process_manager.h"
#include "../ipc_signals/signal_manager.h"

pid_t start_process(const char *program)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        execl(program, program, NULL);

        perror("exec");
        exit(1);
    }

    return pid;
}

pid_t start_process_with_input(
    const char *program,
    const char *input
)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    if (pid == 0) {

        close(pipe_fd[1]);

        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        close(pipe_fd[0]);

        execl(program, program, NULL);

        perror("exec");
        exit(1);
    }

    close(pipe_fd[0]);

    write(
        pipe_fd[1],
        input,
        strlen(input)
    );

    close(pipe_fd[1]);

    return pid;
}

void stop_process(
    pid_t *pid,
    const char *name
)
{
    if (*pid <= 0)
        return;

    printf(
        "[Process Manager] Sending stop signal to %s...\n",
        name
    );

    if (send_signal(*pid, SIGINT) == -1) {

        if (errno != ESRCH)
            perror("signal");
    }

    waitpid(*pid, NULL, 0);

    printf(
        "[Process Manager] %s stopped.\n",
        name
    );

    *pid = -1;
}

int process_running(pid_t pid)
{
    if (pid <= 0)
        return 0;

    if (kill(pid, 0) == 0)
        return 1;

    return 0;
}
