#include <limits.h>
#include <stdlib.h>
#include <stdio.h> 

#include "process.h"
#include "util.h"

// Comparator: sort by Process ID (ascending)
int id_comparer(const void *a, const void *b) { 
    ProcessType *p1 = (ProcessType *)a;
    ProcessType *p2 = (ProcessType *)b;
    return p1->pid - p2->pid;
}

// Comparator: sort by Burst Time (ascending)
int bt_comparer(const void *a, const void *b) { 
    ProcessType *p1 = (ProcessType *)a;
    ProcessType *p2 = (ProcessType *)b;
    return p1->bt - p2->bt;
}

// Comparator: sort by Priority (descending: higher number = higher priority)
int priority_comparer(const void *a, const void *b) {
    ProcessType *p1 = (ProcessType *)a;
    ProcessType *p2 = (ProcessType *)b;
    return p2->pri - p1->pri; // higher number first
}

// FCFS waiting time (handles arrival times)
void findWaitingTimeFCFS(ProcessType plist[], int n) {
    plist[0].wt = 0;
    for (int i = 1; i < n; i++) {
        int finish_prev = plist[i-1].wt + plist[i-1].bt + plist[i-1].art;
        plist[i].wt = (finish_prev > plist[i].art ? finish_prev - plist[i].art : 0);
    }
}

// Turnaround time calculation
void findTurnAroundTime(ProcessType plist[], int n) {
    for (int i = 0; i < n; i++)
        plist[i].tat = plist[i].bt + plist[i].wt;
}

// SRTF / Preemptive SJF waiting time
void findWaitingTimeSJF(ProcessType plist[], int n) {
    int rem_bt[n];
    for (int i = 0; i < n; i++) rem_bt[i] = plist[i].bt;

    int complete = 0, t = 0, min_bt, shortest = -1;
    int finish_time;

    while (complete < n) {
        min_bt = INT_MAX;
        shortest = -1;
        for (int i = 0; i < n; i++) {
            if (plist[i].art <= t && rem_bt[i] > 0 && rem_bt[i] < min_bt) {
                min_bt = rem_bt[i];
                shortest = i;
            }
        }
        if (shortest == -1) {
            t++;
            continue;
        }
        rem_bt[shortest]--;
        if (rem_bt[shortest] == 0) {
            complete++;
            finish_time = t + 1;
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art;
        }
        t++;
    }
}

// Round Robin waiting time (considers arrival time)
// Function to find the waiting time for all processes (Round Robin)
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    int rem_bt[n];       // Remaining burst times
    int wt[n];           // Waiting times
    int t = 0;           // Current time
    int completed = 0;   // Number of processes completed

    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
        wt[i] = 0;
    }

    while (completed < n) {
        int executed_in_this_cycle = 0;

        for (int i = 0; i < n; i++) {
            if (plist[i].art <= t && rem_bt[i] > 0) { // Process has arrived and not finished
                executed_in_this_cycle = 1;

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    wt[i] = t - plist[i].bt - plist[i].art;
                    rem_bt[i] = 0;
                    completed++;
                }
            }
        }

        // If no process was executed, jump to next arrival time
        if (!executed_in_this_cycle) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (rem_bt[i] > 0 && plist[i].art > t) {
                    if (plist[i].art < next_arrival) {
                        next_arrival = plist[i].art;
                    }
                }
            }
            if (next_arrival != INT_MAX)
                t = next_arrival;
        }
    }

    // Copy waiting times to plist
    for (int i = 0; i < n; i++) {
        plist[i].wt = wt[i];
    }
}

// Calculate average times and print heading
void findavgTimeFCFS(ProcessType plist[], int n) {
    findWaitingTimeFCFS(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nFCFS\n");
}

void findavgTimeSJF(ProcessType plist[], int n) {
    findWaitingTimeSJF(plist, n);
    findTurnAroundTime(plist, n);
    qsort(plist, n, sizeof(ProcessType), id_comparer);
    printf("\n*********\nSJF (Preemptive)\n");
}

void findavgTimePriority(ProcessType plist[], int n) {
    qsort(plist, n, sizeof(ProcessType), priority_comparer);
    findWaitingTimeFCFS(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nPriority\n");
}

void findavgTimeRR(ProcessType plist[], int n, int quantum) {
    findWaitingTimeRR(plist, n, quantum);
    findTurnAroundTime(plist, n);
    printf("\n*********\nRound Robin (Quantum = %d)\n", quantum);
}

// Print process table and averages
void printMetrics(ProcessType plist[], int n) {
    int total_wt = 0, total_tat = 0;
    printf("\tPID\tBT\tAT\tPR\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;
        printf("\t%d\t%d\t%d\t%d\t%d\t%d\n",
            plist[i].pid, plist[i].bt, plist[i].art, plist[i].pri, plist[i].wt, plist[i].tat);
    }
    printf("\nAverage waiting time = %.2f\n", (float)total_wt / n);
    printf("Average turnaround time = %.2f\n", (float)total_tat / n);
}

// Initialize process list from file
ProcessType* initProc(char *filename, int *n) {
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid file path\n");
        exit(1);
    }
    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}

// Driver code
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        return 1;
    }

    int quantum = 2;
    int n;
    ProcessType *plist;

    // FCFS
    plist = initProc(argv[1], &n);
    findavgTimeFCFS(plist, n);
    printMetrics(plist, n);
    free(plist);

    // SJF
    plist = initProc(argv[1], &n);
    findavgTimeSJF(plist, n);
    printMetrics(plist, n);
    free(plist);

    // Priority
    plist = initProc(argv[1], &n);
    findavgTimePriority(plist, n);
    printMetrics(plist, n);
    free(plist);

    // Round Robin
    plist = initProc(argv[1], &n);
    findavgTimeRR(plist, n, quantum);
    printMetrics(plist, n);
    free(plist);

    return 0;
}
