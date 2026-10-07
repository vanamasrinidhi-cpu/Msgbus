CC=gcc
CFLAGS=-Wall -Wextra -std=c11
TARGET=msgbus

$(TARGET):
	$(CC) $(CFLAGS) shared_memory_bus/msgbus_cli.c -o shared_memory_bus/msgbus_cli
	$(CC) $(CFLAGS) shared_memory_bus/broker.c -pthread -lrt -o shared_memory_bus/broker
	$(CC) $(CFLAGS) shared_memory_bus/publisher.c -pthread -lrt -o shared_memory_bus/publisher
	$(CC) $(CFLAGS) shared_memory_bus/subscriber.c -pthread -lrt -o shared_memory_bus/subscriber

clean:
	rm -f shared_memory_bus/msgbus_cli
	rm -f shared_memory_bus/broker
	rm -f shared_memory_bus/publisher
	rm -f shared_memory_bus/subscriber

.PHONY: clean
