
#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

void run_fcfs(int requests[], int n, int initial_head) {
    int current = initial_head;
    int total_movement = 0;

    printf("\n========== FCFS DISK SCHEDULING ==========\n");
    printf("Initial head position: %d\n", initial_head);
    printf("Service order: %d", initial_head);

    printf("\n\n%-15s %-15s %-15s\n",
           "Request", "Movement", "Total Movement");

    for (int i = 0; i < n; i++) {
        int movement = abs(requests[i] - current);
        total_movement += movement;

        printf("%-15d %-15d %-15d\n",
               requests[i], movement, total_movement);

        current = requests[i];
    }

    printf("\nOrder serviced: ");

    for (int i = 0; i < n; i++)
        printf("%d%s", requests[i], i == n - 1 ? "" : " -> ");

    printf("\nTotal head movement (FCFS): %d cylinders\n",
           total_movement);
}

void run_sstf(int requests[], int n, int initial_head) {
    int visited[MAX_REQUESTS] = {0};
    int current = initial_head;
    int total_movement = 0;

    printf("\n========== SSTF DISK SCHEDULING ==========\n");
    printf("Initial head position: %d\n", initial_head);

    printf("\n%-15s %-15s %-15s\n",
           "Request", "Movement", "Total Movement");

    printf("%-15s %-15s %-15d\n",
           "Initial", "-", total_movement);

    printf("\nOrder serviced: %d", initial_head);

    for (int count = 0; count < n; count++) {
        int nearest = -1;
        int minimum_distance = 0;

        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                int distance = abs(requests[i] - current);

                if (nearest == -1 || distance < minimum_distance) {
                    minimum_distance = distance;
                    nearest = i;
                }
            }
        }

        int movement = abs(requests[nearest] - current);
        total_movement += movement;

        printf(" -> %d", requests[nearest]);
        printf("\n%-15d %-15d %-15d\n",
               requests[nearest], movement, total_movement);

        current = requests[nearest];
        visited[nearest] = 1;
    }

    printf("\nTotal head movement (SSTF): %d cylinders\n",
           total_movement);
}

int main(void) {
    int n, initial_head;
    int requests[MAX_REQUESTS];

    printf("DISK SCHEDULING SIMULATOR\n");

    printf("Enter number of disk requests: ");

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_REQUESTS) {
        printf("Invalid number of requests.\n");
        return 1;
    }

    printf("Enter initial head position: ");

    if (scanf("%d", &initial_head) != 1 || initial_head < 0) {
        printf("Invalid initial head position.\n");
        return 1;
    }

    printf("Enter the request queue:\n");

    for (int i = 0; i < n; i++) {
        if (scanf("%d", &requests[i]) != 1 || requests[i] < 0) {
            printf("Invalid disk request.\n");
            return 1;
        }
    }

    printf("\nNumber of requests: %d\n", n);

    run_fcfs(requests, n, initial_head);
    run_sstf(requests, n, initial_head);

    return 0;
}
