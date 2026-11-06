CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: scheduler

scheduler: src/scheduler.c
	$(CC) $(CFLAGS) -o scheduler src/scheduler.c

clean:
	rm -f scheduler
