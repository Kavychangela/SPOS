#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Symbol {
    char name[10];
    int addr;
} sym[20];

int sc = 0, lc = 0;

void pass1(char *fname) {
    FILE *fp = fopen(fname, "r");
    FILE *out = fopen("inter.txt", "w");
    FILE *stab = fopen("symtab.txt", "w");
    char l[80], t1[10], t2[10], t3[10];

    if (!fp || !out || !stab) { printf("File error!\n"); exit(0); }

    printf("\n--- PASS 1 ---\n");

    while (fgets(l, 80, fp)) {
        int n = sscanf(l, "%s %s %s", t1, t2, t3);

        if (strcmp(t1, "START") == 0) {
            lc = atoi(t2);
            fprintf(out, "(AD,01)(C,%d)\n", lc);
            printf("(AD,01)(C,%d)\n", lc);
        }
        else if (strcmp(t1, "END") == 0) {
            fprintf(out, "(AD,02)\n");
            printf("(AD,02)\n");
            break;
        }
        else if (strcmp(t2, "DS") == 0) {
            strcpy(sym[sc].name, t1);
            sym[sc++].addr = lc;
            fprintf(out, "(DL,02)(C,%s)\n", t3);
            printf("(DL,02)(C,%s)\n", t3);
            lc += atoi(t3);
        }
        else if (strcmp(t2, "DC") == 0) {
            strcpy(sym[sc].name, t1);
            sym[sc++].addr = lc;
            fprintf(out, "(DL,01)(C,%s)\n", t3);
            printf("(DL,01)(C,%s)\n", t3);
            lc++;
        }
        else {
            fprintf(out, "(IS)(S)\n");
            printf("(IS)(S)\n");
            lc++;
        }
    }

    for (int i = 0; i < sc; i++)
        fprintf(stab, "%s %d\n", sym[i].name, sym[i].addr);

    fclose(fp);
    fclose(out);
    fclose(stab);

    printf("\nSymbol Table:\n");
    for (int i = 0; i < sc; i++)
        printf("%s\t%d\n", sym[i].name, sym[i].addr);

    printf("\nPASS 1 completed.\nGenerated inter.txt and symtab.txt\n");
}

void show(char *f) {
    char x[80];
    FILE *p = fopen(f, "r");
    if (!p) return;
    while (fgets(x, 80, p)) printf("%s", x);
    fclose(p);
}

int main() {
    char fname[30];
    printf("Enter source file name: ");
    scanf("%s", fname);

    printf("\nSource Code:\n");
    show(fname);

    pass1(fname);
    return 0;
}
