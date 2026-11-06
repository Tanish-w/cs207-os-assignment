/**
 * CS207: Operating Systems Design - Assignment 1
 * Part B: CPU Scheduling Simulator
 *
 * Implements FCFS, Non-Preemptive SJF, and Round Robin scheduling.
 * Reads process specifications from a CSV or whitespace-delimited file.
 * Outputs detailed execution metrics and an ASCII Gantt Chart.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROCESSES 128
#define MAX_NAME_LEN 32
#define MAX_TIMELINE_ENTRIES 2048

typedef struct {
    char pid[MAX_NAME_LEN];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int first_start_time;
    bool started;
    bool completed;
} Process;

typedef struct {
    char pid[MAX_NAME_LEN];
    int start_time;
    int end_time;
} GanttSegment;

typedef struct {
    GanttSegment segments[MAX_TIMELINE_ENTRIES];
    int count;
} GanttChart;

/* Helper to append a segment to the Gantt chart, coalescing identical consecutive blocks */
void add_gantt_segment(GanttChart *chart, const char *pid, int start, int end) {
    if (start >= end) return;
    if (chart->count > 0 && strcmp(chart->segments[chart->count - 1].pid, pid) == 0 &&
        chart->segments[chart->count - 1].end_time == start) {
        chart->segments[chart->count - 1].end_time = end;
    } else {
        if (chart->count < MAX_TIMELINE_ENTRIES) {
            strncpy(chart->segments[chart->count].pid, pid, MAX_NAME_LEN - 1);
            chart->segments[chart->count].pid[MAX_NAME_LEN - 1] = '\0';
            chart->segments[chart->count].start_time = start;
            chart->segments[chart->count].end_time = end;
            chart->count++;
        }
    }
}

/* Helper to render an ASCII Gantt Chart */
void print_gantt_chart(const GanttChart *chart) {
    if (chart->count == 0) {
        printf("Gantt Chart is empty.\n");
        return;
    }

    printf("\n--- Gantt Chart ---\n");

    /* Top border */
    printf("+");
    for (int i = 0; i < chart->count; i++) {
        int duration = chart->segments[i].end_time - chart->segments[i].start_time;
        int width = (duration < 2) ? 4 : (duration * 2 > 20 ? 12 : duration * 2 + 2);
        for (int w = 0; w < width; w++) printf("-");
        printf("+");
    }
    printf("\n");

    /* Process names */
    printf("|");
    for (int i = 0; i < chart->count; i++) {
        int duration = chart->segments[i].end_time - chart->segments[i].start_time;
        int width = (duration < 2) ? 4 : (duration * 2 > 20 ? 12 : duration * 2 + 2);
        int name_len = (int)strlen(chart->segments[i].pid);
        int pad_left = (width - name_len) / 2;
        int pad_right = width - name_len - pad_left;
        for (int p = 0; p < pad_left; p++) printf(" ");
        printf("%s", chart->segments[i].pid);
        for (int p = 0; p < pad_right; p++) printf(" ");
        printf("|");
    }
    printf("\n");

    /* Bottom border */
    printf("+");
    for (int i = 0; i < chart->count; i++) {
        int duration = chart->segments[i].end_time - chart->segments[i].start_time;
        int width = (duration < 2) ? 4 : (duration * 2 > 20 ? 12 : duration * 2 + 2);
        for (int w = 0; w < width; w++) printf("-");
        printf("+");
    }
    printf("\n");

    /* Timeline numbers */
    printf("%d", chart->segments[0].start_time);
    char buf[32];

    for (int i = 0; i < chart->count; i++) {
        int duration = chart->segments[i].end_time - chart->segments[i].start_time;
        int width = (duration < 2) ? 4 : (duration * 2 > 20 ? 12 : duration * 2 + 2);
        snprintf(buf, sizeof(buf), "%d", chart->segments[i].end_time);

        int spaces = width + 1 - (int)strlen(buf);
        if (i == 0) {
            spaces = width - (int)strlen(buf);
            if (chart->segments[0].start_time >= 10) spaces += 1;
        }
        if (spaces < 1) spaces = 1;

        for (int s = 0; s < spaces; s++) printf(" ");
        printf("%s", buf);
    }
    printf("\n\n");
}

/* Print performance summary table */
void print_results_table(Process p[], int n, const char *algo_name) {
    printf("========================================================================================\n");
    printf(" Scheduling Algorithm: %s\n", algo_name);
    printf("========================================================================================\n");
    printf("%-8s %-12s %-10s %-16s %-16s %-14s %-14s\n",
           "PID", "Arrival(AT)", "Burst(BT)", "Completion(CT)", "Turnaround(TAT)", "Waiting(WT)", "Response(RT)");
    printf("----------------------------------------------------------------------------------------\n");

    double total_tat = 0.0, total_wt = 0.0, total_rt = 0.0;
    for (int i = 0; i < n; i++) {
        printf("%-8s %-12d %-10d %-16d %-16d %-14d %-14d\n",
               p[i].pid, p[i].arrival_time, p[i].burst_time,
               p[i].completion_time, p[i].turnaround_time,
               p[i].waiting_time, p[i].response_time);
        total_tat += p[i].turnaround_time;
        total_wt += p[i].waiting_time;
        total_rt += p[i].response_time;
    }
    printf("----------------------------------------------------------------------------------------\n");
    printf("Average Turnaround Time (TAT) : %.2f\n", total_tat / n);
    printf("Average Waiting Time (WT)    : %.2f\n", total_wt / n);
    printf("Average Response Time (RT)   : %.2f\n", total_rt / n);
    printf("========================================================================================\n");
}

/* Reset process transient fields before running an algorithm */
void reset_processes(const Process src[], Process dst[], int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
        dst[i].remaining_time = src[i].burst_time;
        dst[i].completion_time = 0;
        dst[i].turnaround_time = 0;
        dst[i].waiting_time = 0;
        dst[i].response_time = 0;
        dst[i].first_start_time = -1;
        dst[i].started = false;
        dst[i].completed = false;
    }
}

/* 1. First-Come First-Served (FCFS) */
void simulate_fcfs(const Process original[], int n) {
    Process p[MAX_PROCESSES];
    reset_processes(original, p, n);
    GanttChart chart = { .count = 0 };

    int completed = 0;
    int current_time = 0;

    while (completed < n) {
        int best_idx = -1;
        int earliest_arrival = 1e9;

        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                if (p[i].arrival_time < earliest_arrival) {
                    earliest_arrival = p[i].arrival_time;
                    best_idx = i;
                }
            }
        }

        if (best_idx == -1) {
            /* CPU is idle; find next arriving process */
            int next_arr = 1e9;
            for (int i = 0; i < n; i++) {
                if (!p[i].completed && p[i].arrival_time < next_arr) {
                    next_arr = p[i].arrival_time;
                }
            }
            add_gantt_segment(&chart, "[IDLE]", current_time, next_arr);
            current_time = next_arr;
            continue;
        }

        int idx = best_idx;
        p[idx].first_start_time = current_time;
        p[idx].response_time = p[idx].first_start_time - p[idx].arrival_time;
        add_gantt_segment(&chart, p[idx].pid, current_time, current_time + p[idx].burst_time);

        current_time += p[idx].burst_time;
        p[idx].completion_time = current_time;
        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
        p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
        p[idx].completed = true;
        completed++;
    }

    print_results_table(p, n, "First-Come First-Served (FCFS)");
    print_gantt_chart(&chart);
}

/* 2. Non-Preemptive Shortest Job First (SJF) */
void simulate_sjf(const Process original[], int n) {
    Process p[MAX_PROCESSES];
    reset_processes(original, p, n);
    GanttChart chart = { .count = 0 };

    int completed = 0;
    int current_time = 0;

    while (completed < n) {
        int best_idx = -1;
        int shortest_burst = 1e9;

        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                if (p[i].burst_time < shortest_burst) {
                    shortest_burst = p[i].burst_time;
                    best_idx = i;
                } else if (p[i].burst_time == shortest_burst) {
                    /* Break ties by arrival time */
                    if (best_idx != -1 && p[i].arrival_time < p[best_idx].arrival_time) {
                        best_idx = i;
                    }
                }
            }
        }

        if (best_idx == -1) {
            int next_arr = 1e9;
            for (int i = 0; i < n; i++) {
                if (!p[i].completed && p[i].arrival_time < next_arr) {
                    next_arr = p[i].arrival_time;
                }
            }
            add_gantt_segment(&chart, "[IDLE]", current_time, next_arr);
            current_time = next_arr;
            continue;
        }

        int idx = best_idx;
        p[idx].first_start_time = current_time;
        p[idx].response_time = p[idx].first_start_time - p[idx].arrival_time;
        add_gantt_segment(&chart, p[idx].pid, current_time, current_time + p[idx].burst_time);

        current_time += p[idx].burst_time;
        p[idx].completion_time = current_time;
        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
        p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
        p[idx].completed = true;
        completed++;
    }

    print_results_table(p, n, "Shortest Job First (SJF - Non-Preemptive)");
    print_gantt_chart(&chart);
}

/* 3. Round Robin (RR) */
void simulate_round_robin(const Process original[], int n, int quantum) {
    Process p[MAX_PROCESSES];
    reset_processes(original, p, n);
    GanttChart chart = { .count = 0 };

    int ready_queue[MAX_TIMELINE_ENTRIES];
    int front = 0, rear = 0;
    bool in_queue[MAX_PROCESSES] = { false };

    int completed = 0;
    int current_time = 0;

    /* Find minimum arrival time to initialize simulation clock */
    int min_arr = 1e9;
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time < min_arr) min_arr = p[i].arrival_time;
    }
    if (min_arr > 0) {
        add_gantt_segment(&chart, "[IDLE]", 0, min_arr);
        current_time = min_arr;
    }

    /* Enqueue processes arriving at current_time */
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time && !in_queue[i]) {
            ready_queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    while (completed < n) {
        if (front == rear) {
            /* Queue empty, fast-forward time to next arriving process */
            int next_arr = 1e9;
            for (int i = 0; i < n; i++) {
                if (!p[i].completed && p[i].arrival_time < next_arr) {
                    next_arr = p[i].arrival_time;
                }
            }
            if (next_arr < 1e9) {
                add_gantt_segment(&chart, "[IDLE]", current_time, next_arr);
                current_time = next_arr;
                for (int i = 0; i < n; i++) {
                    if (p[i].arrival_time <= current_time && !in_queue[i] && !p[i].completed) {
                        ready_queue[rear++] = i;
                        in_queue[i] = true;
                    }
                }
            }
            continue;
        }

        int idx = ready_queue[front++];
        in_queue[idx] = false;

        if (!p[idx].started) {
            p[idx].started = true;
            p[idx].first_start_time = current_time;
            p[idx].response_time = p[idx].first_start_time - p[idx].arrival_time;
        }

        int exec_time = (p[idx].remaining_time < quantum) ? p[idx].remaining_time : quantum;
        add_gantt_segment(&chart, p[idx].pid, current_time, current_time + exec_time);

        current_time += exec_time;
        p[idx].remaining_time -= exec_time;

        /* Enqueue any new processes that arrived while current process was executing */
        for (int i = 0; i < n; i++) {
            if (i != idx && !p[i].completed && !in_queue[i] && p[i].arrival_time <= current_time) {
                ready_queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        /* If process still has remaining time, push back to queue; otherwise mark complete */
        if (p[idx].remaining_time > 0) {
            ready_queue[rear++] = idx;
            in_queue[idx] = true;
        } else {
            p[idx].completed = true;
            p[idx].completion_time = current_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            completed++;
        }
    }

    char title[128];
    snprintf(title, sizeof(title), "Round Robin (Time Quantum = %d)", quantum);
    print_results_table(p, n, title);
    print_gantt_chart(&chart);
}

/* Load process table from CSV/whitespace file */
int load_processes(const char *filename, Process p[], int max_p) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* Strip newline and whitespace */
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0' || *ptr == '#' || *ptr == '\n' || *ptr == '\r') continue;

        /* Skip header line if detected */
        if (strncmp(ptr, "PID", 3) == 0 || strncmp(ptr, "pid", 3) == 0) continue;

        char pid[MAX_NAME_LEN];
        int at, bt;

        /* Try comma-separated first, then whitespace */
        if (sscanf(ptr, "%31[^,],%d,%d", pid, &at, &bt) == 3 ||
            sscanf(ptr, "%31s %d %d", pid, &at, &bt) == 3) {
            if (count >= max_p) {
                fprintf(stderr, "Warning: Maximum process limit (%d) reached.\n", max_p);
                break;
            }
            /* Trim spaces from pid */
            char *trim_end = pid + strlen(pid) - 1;
            while (trim_end > pid && (*trim_end == ' ' || *trim_end == '\t')) {
                *trim_end = '\0';
                trim_end--;
            }

            strncpy(p[count].pid, pid, MAX_NAME_LEN - 1);
            p[count].pid[MAX_NAME_LEN - 1] = '\0';
            p[count].arrival_time = at;
            p[count].burst_time = bt;
            p[count].remaining_time = bt;
            p[count].first_start_time = -1;
            p[count].started = false;
            p[count].completed = false;
            count++;
        }
    }

    fclose(fp);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <process_file.csv> [time_quantum]\n", argv[0]);
        printf("Example: %s test_cases/processes_part_b.csv 3\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    int quantum = 2; // Default quantum
    if (argc >= 3) {
        quantum = atoi(argv[2]);
        if (quantum <= 0) {
            fprintf(stderr, "Error: Time quantum must be a positive integer. Using default 2.\n");
            quantum = 2;
        }
    }

    Process processes[MAX_PROCESSES];
    int n = load_processes(filepath, processes, MAX_PROCESSES);
    if (n <= 0) {
        fprintf(stderr, "Error: No valid processes loaded from '%s'.\n", filepath);
        return 1;
    }

    printf("========================================================================================\n");
    printf(" CS207: OPERATING SYSTEMS DESIGN - CPU SCHEDULING SIMULATOR\n");
    printf(" Loaded %d processes from '%s'. Configured Time Quantum for Round Robin = %d\n", n, filepath, quantum);
    printf("========================================================================================\n\n");

    simulate_fcfs(processes, n);
    simulate_sjf(processes, n);
    simulate_round_robin(processes, n, quantum);

    return 0;
}
