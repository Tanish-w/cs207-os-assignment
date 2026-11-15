
#include <stdio.h>

#define MAX_P 8
#define MAX_R 10

int n, m;

int allocation[MAX_P][MAX_R];
int maximum[MAX_P][MAX_R];
int need[MAX_P][MAX_R];

int available[MAX_R];
int work[MAX_R];

int finish[MAX_P] = {0};
int sequence[MAX_P];

unsigned long long total_sequences = 0;

/* Print one safe sequence */
void print_sequence(int depth)
{
    total_sequences++;

    printf("Safe Sequence %llu: ", total_sequences);

    for (int i = 0; i < depth; i++) {
        printf("P%d", sequence[i]);

        if (i != depth - 1)
            printf(" -> ");
    }

    printf("\n");
}

/* Find and print all possible safe sequences */
void find_safe_sequences(int depth)
{
    if (depth == n) {
        print_sequence(depth);
        return;
    }

    for (int i = 0; i < n; i++) {

        if (finish[i])
            continue;

        int can_finish = 1;

        for (int j = 0; j < m; j++) {
            if (need[i][j] > work[j]) {
                can_finish = 0;
                break;
            }
        }

        if (can_finish) {

            finish[i] = 1;
            sequence[depth] = i;

            for (int j = 0; j < m; j++)
                work[j] += allocation[i][j];

            find_safe_sequences(depth + 1);

            /* Backtracking */
            for (int j = 0; j < m; j++)
                work[j] -= allocation[i][j];

            finish[i] = 0;
        }
    }
}

/* Explain why an unsafe state has no safe sequence */
void explain_unsafe_state(void)
{
    int diagnostic_work[MAX_R];
    int diagnostic_finish[MAX_P] = {0};

    int progress;

    for (int j = 0; j < m; j++)
        diagnostic_work[j] = available[j];

    /* Simulate completion of processes that can finish */
    do {
        progress = 0;

        for (int i = 0; i < n; i++) {

            if (diagnostic_finish[i])
                continue;

            int can_finish = 1;

            for (int j = 0; j < m; j++) {
                if (need[i][j] > diagnostic_work[j]) {
                    can_finish = 0;
                    break;
                }
            }

            if (can_finish) {

                for (int j = 0; j < m; j++)
                    diagnostic_work[j] += allocation[i][j];

                diagnostic_finish[i] = 1;
                progress = 1;
            }
        }

    } while (progress);

    printf("\nWHY IS THE STATE UNSAFE?\n");

    printf("The following processes cannot be guaranteed to complete:\n");

    for (int i = 0; i < n; i++) {

        if (!diagnostic_finish[i]) {

            printf("\nP%d:\n", i);

            for (int j = 0; j < m; j++) {

                printf("Resource R%d: Need = %d, Available Work = %d\n",
                       j, need[i][j], diagnostic_work[j]);
            }
        }
    }

    printf("\nNo unfinished process can satisfy its remaining "
           "resource requirements.\n");

    printf("Therefore, no safe sequence exists.\n");

    printf("An unsafe state does not necessarily mean "
           "deadlock has already occurred.\n");
}

int main(void)
{
    printf("BANKER'S ALGORITHM - DEADLOCK AVOIDANCE\n\n");

    /* Read number of processes */
    printf("Enter number of processes (max %d): ", MAX_P);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_P) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    /* Read number of resource types */
    printf("Enter number of resource types (max %d): ", MAX_R);

    if (scanf("%d", &m) != 1 || m < 1 || m > MAX_R) {
        printf("Invalid number of resource types.\n");
        return 1;
    }

    /* Read Allocation Matrix */
    printf("\nEnter Allocation Matrix (%d x %d):\n", n, m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {

            if (scanf("%d", &allocation[i][j]) != 1 ||
                allocation[i][j] < 0) {

                printf("Invalid allocation value.\n");
                return 1;
            }
        }
    }

    /* Read Maximum Matrix */
    printf("\nEnter Maximum Matrix (%d x %d):\n", n, m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {

            if (scanf("%d", &maximum[i][j]) != 1 ||
                maximum[i][j] < allocation[i][j]) {

                printf("Invalid maximum value. "
                       "Maximum must be >= Allocation.\n");

                return 1;
            }

            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }

    /* Read Available Resources */
    printf("\nEnter Available Resources (%d values):\n", m);

    for (int j = 0; j < m; j++) {

        if (scanf("%d", &available[j]) != 1 ||
            available[j] < 0) {

            printf("Invalid available resource value.\n");
            return 1;
        }

        work[j] = available[j];
    }

    /* Display Need Matrix */
    printf("\nNeed Matrix:\n");

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < m; j++)
            printf("%d ", need[i][j]);

        printf("\n");
    }

    /* Run Banker's Safety Algorithm */
    printf("\nChecking system safety...\n\n");

    find_safe_sequences(0);

    if (total_sequences > 0) {

        printf("\nSYSTEM IS IN A SAFE STATE.\n");

        printf("Total safe sequences: %llu\n", total_sequences);

    } else {

        printf("\nNO SAFE SEQUENCE EXISTS.\n");

        printf("SYSTEM IS IN AN UNSAFE STATE.\n");

        explain_unsafe_state();
    }

    return 0;
}
