CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11
TARGET=msgbus

all: $(TARGET) shared_memory

$(TARGET): msgbus.c process_control/process_manager.c ipc_signals/signal_manager.c
	$(CC) $(CFLAGS) msgbus.c process_control/process_manager.c ipc_signals/signal_manager.c -o $(TARGET)

shared_memory:
	$(CC) $(CFLAGS) shared_memory_bus/broker.c ipc_signals/signal_manager.c -pthread -lrt -o shared_memory_bus/broker
	$(CC) $(CFLAGS) shared_memory_bus/publisher.c -pthread -lrt -o shared_memory_bus/publisher
	$(CC) $(CFLAGS) shared_memory_bus/subscriber.c ipc_signals/signal_manager.c -pthread -lrt -o shared_memory_bus/subscriber

clean:
	rm -f $(TARGET)
	rm -f shared_memory_bus/broker
	rm -f shared_memory_bus/publisher
	rm -f shared_memory_bus/subscriber
	rm -f shared_memory_bus/msgbus_cli

.PHONY: all shared_memory clean
