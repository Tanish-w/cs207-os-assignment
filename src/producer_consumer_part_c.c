
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 100
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

typedef struct {
    int id;
    int producer_id;
} Item;

Item buffer[BUFFER_SIZE];

int in = 0;
int out = 0;
int buffer_count = 0;

int consumed[TOTAL_ITEMS + 1] = {0};
int consumed_total = 0;
int duplicate_count = 0;

sem_t empty_slots;
sem_t full_slots;

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verification_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void wait_semaphore(sem_t *sem) {
    while (sem_wait(sem) == -1) {
        if (errno != EINTR) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }
}

void insert_item(Item item) {
    wait_semaphore(&empty_slots);

    pthread_mutex_lock(&buffer_mutex);

    if (buffer_count >= BUFFER_SIZE) {
        fprintf(stderr, "ERROR: Buffer overflow detected!\n");
        exit(EXIT_FAILURE);
    }

    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    buffer_count++;

    pthread_mutex_unlock(&buffer_mutex);

    sem_post(&full_slots);
}

Item remove_item(void) {
    wait_semaphore(&full_slots);

    pthread_mutex_lock(&buffer_mutex);

    if (buffer_count <= 0) {
        fprintf(stderr, "ERROR: Buffer underflow detected!\n");
        exit(EXIT_FAILURE);
    }

    Item item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    buffer_count--;

    pthread_mutex_unlock(&buffer_mutex);

    sem_post(&empty_slots);

    return item;
}

void *producer(void *arg) {
    int producer_id = *(int *)arg;

    for (int id = producer_id; id <= TOTAL_ITEMS;
         id += NUM_PRODUCERS) {

        Item item = {id, producer_id};

        insert_item(item);

        pthread_mutex_lock(&log_mutex);
        printf("Producer %d produced Item %d\n",
               producer_id, id);
        pthread_mutex_unlock(&log_mutex);
    }

    return NULL;
}

void *consumer(void *arg) {
    int consumer_id = *(int *)arg;

    while (1) {
        Item item = remove_item();

        if (item.id == -1) {
            pthread_mutex_lock(&log_mutex);
            printf("Consumer %d received termination signal\n",
                   consumer_id);
            pthread_mutex_unlock(&log_mutex);
            break;
        }

        pthread_mutex_lock(&verification_mutex);

        if (item.id < 1 || item.id > TOTAL_ITEMS) {
            duplicate_count++;
        } else {
            consumed[item.id]++;

            if (consumed[item.id] > 1) {
                duplicate_count++;
            }
        }

        consumed_total++;

        pthread_mutex_unlock(&verification_mutex);

        pthread_mutex_lock(&log_mutex);
        printf("Consumer %d consumed Item %d (from Producer %d)\n",
               consumer_id, item.id, item.producer_id);
        pthread_mutex_unlock(&log_mutex);
    }

    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    int producer_ids[NUM_PRODUCERS] = {1, 2};
    int consumer_ids[NUM_CONSUMERS] = {1, 2};

    if (sem_init(&empty_slots, 0, BUFFER_SIZE) != 0 ||
        sem_init(&full_slots, 0, 0) != 0) {
        perror("sem_init");
        return EXIT_FAILURE;
    }

    printf("PRODUCER-CONSUMER PROBLEM\n");
    printf("Buffer capacity: %d\n", BUFFER_SIZE);
    printf("Producers: %d | Consumers: %d\n", 
           NUM_PRODUCERS, NUM_CONSUMERS);
    printf("Total items: %d\n\n", TOTAL_ITEMS);

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_create(&consumers[i], NULL,
                           consumer, &consumer_ids[i]) != 0) {
            perror("pthread_create consumer");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_create(&producers[i], NULL,
                           producer, &producer_ids[i]) != 0) {
            perror("pthread_create producer");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    /* Send one termination signal to each consumer. */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        Item stop = {-1, -1};
        insert_item(stop);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    int missing = 0;

    for (int i = 1; i <= TOTAL_ITEMS; i++) {
        if (consumed[i] != 1) {
            missing++;
        }
    }

    printf("\n========== FINAL VERIFICATION ==========\n");
    printf("Total items produced: %d\n", TOTAL_ITEMS);
    printf("Total items consumed: %d\n", consumed_total);
    printf("Missing or duplicate items: %d\n", missing + duplicate_count);
    printf("Final buffer count: %d\n", buffer_count);

    if (consumed_total == TOTAL_ITEMS &&
        missing == 0 &&
        duplicate_count == 0 &&
        buffer_count == 0) {
        printf("SUCCESS: Every item was consumed exactly once.\n");
        printf("No buffer overflow or underflow occurred.\n");
    } else {
        printf("FAILURE: Verification did not pass.\n");
    }

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&verification_mutex);
    pthread_mutex_destroy(&log_mutex);

    return (consumed_total == TOTAL_ITEMS &&
            missing == 0 &&
            duplicate_count == 0 &&
            buffer_count == 0)
           ? EXIT_SUCCESS : EXIT_FAILURE;
}
