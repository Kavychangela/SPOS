#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Symbol { char name[10]; int addr; } sym[20];
int sc = 0;

// Load symbol table
void loadSymtab() {
    FILE *fp = fopen("symtab.txt", "r");
    if (!fp) { printf("Missing symtab.txt! Run Pass 1 first.\n"); exit(0); }
    while (fscanf(fp, "%s %d", sym[sc].name, &sym[sc].addr) == 2)
        sc++;
    fclose(fp);
}

int findSymbol(char *name) {
    for (int i = 0; i < sc; i++)
        if (strcmp(sym[i].name, name) == 0)
            return sym[i].addr;
    return -1;
}

void pass2() {
    FILE *fp = fopen("inter.txt", "r");
    if (!fp) { printf("Missing inter.txt! Run Pass 1 first.\n"); exit(0); }

    loadSymtab();

    printf("\n--- PASS 2 OUTPUT ---\n");
    char line[80];
    int loc = 0;

    // find starting address from first line
    if (fgets(line, 80, fp)) {
        if (strstr(line, "(AD,01)")) {
            sscanf(line, "(AD,01)(C,%d)", &loc);
        } else rewind(fp); // if no START
    }

    // process rest of intermediate code
    while (fgets(line, 80, fp)) {
        if (strstr(line, "(IS)")) {
            printf("Machine code at %d -> Instruction Placeholder\n", loc++);
        }
        else if (strstr(line, "(DL,01)") || strstr(line, "(DL,02)")) {
            loc++;
        }
    }

    fclose(fp);
    printf("\nPASS 2 completed.\n");
}

int main() {
    pass2();
    return 0;
}
