CC = gcc
CFLAGS = -Wall -Wextra -std=c11
PTHREAD = -pthread

BINARIES = scheduler producer_consumer bankers page_replacement disk_scheduling

all: $(BINARIES)

scheduler: src/scheduler.c
	$(CC) $(CFLAGS) -o $@ $<

producer_consumer: src/producer_consumer_part_c.c
	$(CC) $(CFLAGS) $(PTHREAD) -o $@ $<

bankers: src/bankers_part_d.c
	$(CC) $(CFLAGS) -o $@ $<

page_replacement: src/page_replacement_part_e.c
	$(CC) $(CFLAGS) -o $@ $<

disk_scheduling: src/disk_scheduling_part_e.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINARIES)
