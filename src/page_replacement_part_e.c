
#include <stdio.h>
#include <stdlib.h>

#define MAX_PAGES 100
#define MAX_FRAMES 20

void run_fifo(int pages[], int n, int frame_count) {
    int frames[MAX_FRAMES];
    int next = 0;
    int faults = 0;

    for (int i = 0; i < frame_count; i++)
        frames[i] = -1;

    printf("\n========== FIFO PAGE REPLACEMENT ==========\n");
    printf("%-10s", "Page");

    for (int i = 0; i < frame_count; i++)
        printf("F%-9d", i + 1);

    printf("%-12s\n", "Status");

    for (int i = 0; i < n; i++) {
        int hit = 0;

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == pages[i]) {
                hit = 1;
                break;
            }
        }

        if (!hit) {
            frames[next] = pages[i];
            next = (next + 1) % frame_count;
            faults++;
        }

        printf("%-10d", pages[i]);

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == -1)
                printf("%-10s", "-");
            else
                printf("%-10d", frames[j]);
        }

        printf("%-12s\n", hit ? "HIT" : "FAULT");
    }

    printf("Total page faults (FIFO): %d\n", faults);
}

void run_lru(int pages[], int n, int frame_count) {
    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES];
    int faults = 0;

    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1;
        last_used[i] = -1;
    }

    printf("\n========== LRU PAGE REPLACEMENT ==========\n");
    printf("%-10s", "Page");

    for (int i = 0; i < frame_count; i++)
        printf("F%-9d", i + 1);

    printf("%-12s\n", "Status");

    for (int i = 0; i < n; i++) {
        int hit = 0;
        int position = -1;

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == pages[i]) {
                hit = 1;
                position = j;
                break;
            }
        }

        if (hit) {
            last_used[position] = i;
        } else {
            int lru_position = -1;

            for (int j = 0; j < frame_count; j++) {
                if (frames[j] == -1) {
                    lru_position = j;
                    break;
                }
            }

            if (lru_position == -1) {
                lru_position = 0;

                for (int j = 1; j < frame_count; j++) {
                    if (last_used[j] < last_used[lru_position])
                        lru_position = j;
                }
            }

            frames[lru_position] = pages[i];
            last_used[lru_position] = i;
            faults++;
        }

        printf("%-10d", pages[i]);

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == -1)
                printf("%-10s", "-");
            else
                printf("%-10d", frames[j]);
        }

        printf("%-12s\n", hit ? "HIT" : "FAULT");
    }

    printf("Total page faults (LRU): %d\n", faults);
}

int main(void) {
    int pages[MAX_PAGES];
    int n, frame_count;

    printf("PAGE REPLACEMENT SIMULATOR\n");
    printf("Enter number of page references: ");

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_PAGES) {
        printf("Invalid number of page references.\n");
        return 1;
    }

    printf("Enter the page reference string:\n");

    for (int i = 0; i < n; i++) {
        if (scanf("%d", &pages[i]) != 1 || pages[i] < 0) {
            printf("Invalid page reference.\n");
            return 1;
        }
    }

    printf("Enter number of frames (1-%d): ", MAX_FRAMES);

    if (scanf("%d", &frame_count) != 1 ||
        frame_count < 1 || frame_count > MAX_FRAMES) {
        printf("Invalid frame count.\n");
        return 1;
    }

    printf("\nNumber of references: %d\n", n);
    printf("Number of frames: %d\n", frame_count);

    run_fifo(pages, n, frame_count);
    run_lru(pages, n, frame_count);

    return 0;
}
