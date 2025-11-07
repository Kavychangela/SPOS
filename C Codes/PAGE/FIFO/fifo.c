#include <stdio.h>

int main() {
    int n, f, pages[50], frames[10], next = 0, faults = 0;
    printf("Enter number of pages: ");
    scanf("%d", &n);

    printf("Enter page reference string:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &pages[i]);

    printf("Enter number of frames: ");
    scanf("%d", &f);

    for (int i = 0; i < f; i++)
        frames[i] = -1;

    printf("\n--- FIFO Page Replacement ---\n");

    for (int i = 0; i < n; i++) {
        int hit = 0;
        for (int j = 0; j < f; j++)
            if (frames[j] == pages[i]) hit = 1;

        if (!hit) {
            frames[next] = pages[i];
            next = (next + 1) % f;
            faults++;
        }

        printf("Page %2d -> ", pages[i]);
        for (int j = 0; j < f; j++)
            if (frames[j] != -1)
                printf("%2d ", frames[j]);
            else
                printf("-  ");
        printf(hit ? "(Hit)\n" : "(Fault)\n");
    }

    printf("\nTotal Page Faults = %d\n", faults);
    printf("Total Page Hits   = %d\n", n - faults);
    return 0;
}
