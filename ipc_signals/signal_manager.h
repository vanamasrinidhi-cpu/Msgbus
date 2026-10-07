#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include <sys/types.h>

int register_signal(int signal_number, void (*handler)(int));
int send_signal(pid_t pid, int signal_number);

#endif
