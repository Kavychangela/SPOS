#include <stdio.h>
#include <string.h>

struct Process {
    char name[5];
    int bt, at, wt, tat, rt, rem_bt;
};

int main() {
    int n, tq;
    struct Process p[10];
    float avg_wt = 0, avg_tat = 0, avg_rt = 0;
    int time = 0, completed = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);
    printf("Enter time quantum: ");
    scanf("%d", &tq);

    for (int i = 0; i < n; i++) {
        sprintf(p[i].name, "P%d", i + 1);
        printf("Enter burst time and arrival time for %s: ", p[i].name);
        scanf("%d %d", &p[i].bt, &p[i].at);
        p[i].rem_bt = p[i].bt;
        p[i].wt = p[i].tat = -1;
        p[i].rt = -1;
    }

    // Round Robin execution
    while (completed < n) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (p[i].at <= time && p[i].rem_bt > 0) {
                done = 0;

                // first response
                if (p[i].rt == -1)
                    p[i].rt = time - p[i].at;

                if (p[i].rem_bt > tq) {
                    time += tq;
                    p[i].rem_bt -= tq;
                } else {
                    time += p[i].rem_bt;
                    p[i].rem_bt = 0;
                    completed++;

                    p[i].tat = time - p[i].at;
                    p[i].wt = p[i].tat - p[i].bt;
                }
            }
        }
        if (done) time++; // no process ready, idle CPU
    }

    // Results
    printf("\nProcess\tAT\tBT\tWT\tTAT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("%s\t%d\t%d\t%d\t%d\t%d\n", p[i].name, p[i].at, p[i].bt,
               p[i].wt, p[i].tat, p[i].rt);
        avg_wt += p[i].wt;
        avg_tat += p[i].tat;
        avg_rt += p[i].rt;
    }

    printf("\nAverage WT = %.2f", avg_wt / n);
    printf("\nAverage TAT = %.2f", avg_tat / n);
    printf("\nAverage RT = %.2f\n", avg_rt / n);
    return 0;
}
