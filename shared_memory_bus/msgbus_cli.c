#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_HISTORY 20
#define MAX_COMMAND 100

char history[MAX_HISTORY][MAX_COMMAND];
int history_count = 0;

void show_help() {
    printf("\n===== MsgBus Commands =====\n");
    printf("  start       - Show broker start command\n");
    printf("  publisher   - Show publisher command\n");
    printf("  subscriber  - Show subscriber command\n");
    printf("  log         - Show log file command\n");
    printf("  status      - Show current process ID\n");
    printf("  help        - Show commands\n");
    printf("  exit        - Exit interface\n");
}

void add_history(char *command) {

    if (strlen(command) == 0)
        return;

    if (history_count < MAX_HISTORY) {

        strcpy(history[history_count], command);
        history_count++;

    } else {

        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }

        strcpy(history[MAX_HISTORY - 1], command);
    }
}

void clear_line(char *command) {

    printf("\33[2K\r");
    printf("msgbus> %s", command);

    fflush(stdout);
}

void read_command(char *command) {

    struct termios old_terminal;
    struct termios new_terminal;

    tcgetattr(STDIN_FILENO, &old_terminal);

    new_terminal = old_terminal;

    new_terminal.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &new_terminal
    );

    int position = 0;
    int history_position = history_count;

    command[0] = '\0';

    printf("msgbus> ");
    fflush(stdout);

    while (1) {

        char c;

        if (read(STDIN_FILENO, &c, 1) != 1)
            continue;

        /* Enter key */
        if (c == '\n' || c == '\r') {

            command[position] = '\0';

            printf("\n");

            break;
        }

        /* Backspace */
        if (c == 127 || c == 8) {

            if (position > 0) {

                position--;

                command[position] = '\0';

                printf("\b \b");

                fflush(stdout);
            }

            continue;
        }

        /* Arrow keys */
        if (c == 27) {

            char sequence[2];

            if (read(STDIN_FILENO, &sequence[0], 1) != 1)
                continue;

            if (read(STDIN_FILENO, &sequence[1], 1) != 1)
                continue;

            /* Up arrow */
            if (sequence[0] == '[' &&
                sequence[1] == 'A') {

                if (history_count > 0 &&
                    history_position > 0) {

                    history_position--;

                    strcpy(
                        command,
                        history[history_position]
                    );

                    position = strlen(command);

                    clear_line(command);
                }
            }

            /* Down arrow */
            else if (sequence[0] == '[' &&
                     sequence[1] == 'B') {

                if (history_position < history_count - 1) {

                    history_position++;

                    strcpy(
                        command,
                        history[history_position]
                    );

                    position = strlen(command);

                    clear_line(command);
                }
                else if (history_position ==
                         history_count - 1) {

                    history_position = history_count;

                    command[0] = '\0';

                    position = 0;

                    clear_line(command);
                }
            }

            continue;
        }

        /* Normal character */
        if (c >= 32 && c <= 126) {

            if (position < MAX_COMMAND - 1) {

                command[position] = c;

                position++;

                command[position] = '\0';

                printf("%c", c);

                fflush(stdout);
            }
        }
    }

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &old_terminal
    );
}

int main() {

    char command[MAX_COMMAND];

    printf("====================================\n");
    printf("          MsgBus Interface           \n");
    printf("====================================\n");

    printf("MsgBus Interface PID: %d\n", getpid());

    printf("Type 'help' to see commands.\n");
    printf("Use UP/DOWN arrows for command history.\n\n");

    while (1) {

        read_command(command);

        if (strlen(command) == 0)
            continue;

        add_history(command);

        if (strcmp(command, "help") == 0) {

            show_help();
        }

        else if (strcmp(command, "start") == 0) {

            printf(
                "Start broker using: ./broker\n"
            );
        }

        else if (strcmp(command, "publisher") == 0) {

            printf(
                "Start publisher using: ./publisher\n"
            );
        }

        else if (strcmp(command, "subscriber") == 0) {

            printf(
                "Start subscriber using: ./subscriber\n"
            );
        }

        else if (strcmp(command, "log") == 0) {

            printf(
                "View messages using: cat msgbus.log\n"
            );
        }

        else if (strcmp(command, "status") == 0) {

            printf(
                "Interface PID: %d\n",
                getpid()
            );
        }

        else if (strcmp(command, "exit") == 0) {

            printf("MsgBus interface stopped.\n");

            break;
        }

        else {

            printf(
                "Unknown command. Type 'help'.\n"
            );
        }
    }

    return 0;
}