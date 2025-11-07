#include <stdio.h>
#define INFINITE 1000

int search(int a[], int n, int pageno) {
    for (int i = 0; i < n; i++)
        if (a[i] == pageno)
            return i; // return index if found
    return -1;
}

int findempty(int a[], int n) {
    for (int i = 0; i < n; i++)
        if (a[i] == -1)
            return i;
    return -1;
}

int findmin(int a[], int n) { // for MRU, we remove the most recently used
    int i, j = 0;
    for (i = 1; i < n; i++)
        if (a[i] < a[j])
            j = i;
    return j;
}

int main() {
    int frames[10], age[10], trace[30];
    int nframes, ntrace, i, j, loc, page_faults = 0;

    printf("\nEnter no. of frames: ");
    scanf("%d", &nframes);
    printf("\nEnter no. of entries in the page trace: ");
    scanf("%d", &ntrace);
    printf("\nEnter page trace: ");
    for (i = 0; i < ntrace; i++)
        scanf("%d", &trace[i]);

    for (i = 0; i < nframes; i++) {
        frames[i] = -1;
        age[i] = 0;
    }

    printf("\nPage No.\tMRU Allocation\n");

    for (i = 0; i < ntrace; i++) {
        int found = search(frames, nframes, trace[i]);

        // Update age counters
        for (j = 0; j < nframes; j++)
            if (frames[j] != -1)
                age[j]++;

        if (found == -1) { // page fault
            loc = findempty(frames, nframes);
            if (loc == -1) {
                loc = findmin(age, nframes); // replace most recently used (smallest age)
            }
            frames[loc] = trace[i];
            age[loc] = 0;
            page_faults++;
        } else {
            age[found] = 0; // reset age for the page just accessed
        }

        printf("%d\t\t", trace[i]);
        for (j = 0; j < nframes; j++)
            printf("%3d ", frames[j]);
        printf("\n");
    }

    printf("\nTotal Page Faults: %d\n", page_faults);
    return 0;
}
