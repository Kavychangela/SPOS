#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---------- Config ---------- */
#define MAX_SYM  100
#define MAX_LINE 128

/* ---------- Symbol Table ---------- */
struct Symbol { char name[20]; int addr; } sym[MAX_SYM];
int sc = 0;            /* symbol count */
int LC = 0;            /* location counter */
int START_ADDR = 0;

/* ---------- MOT & REG ---------- */
struct MOT { char op[10]; int code; int needReg; int needMem; } mot[] = {
    {"MOVER", 1, 1, 1}, {"MOVEM", 2, 1, 1}, {"ADD", 3, 1, 1},
    {"SUB",   4, 1, 1}, {"COMP", 5, 1, 1}, {"READ",6, 0, 1},
    {"PRINT", 7, 0, 1}, {"STOP", 0, 0, 0}
};
int MOTN = sizeof(mot)/sizeof(mot[0]);

struct REG { char r[3]; int code; } regtab[] = {
    {"A",1},{"B",2},{"C",3},{"D",4}
};
int REGN = sizeof(regtab)/sizeof(regtab[0]);

/* ---------- Helpers ---------- */
int isblankline(const char *s){
    while(*s){ if(!isspace((unsigned char)*s)) return 0; s++; }
    return 1;
}
int regcode(const char* r){
    for(int i=0;i<REGN;i++) if(strcmp(regtab[i].r,r)==0) return regtab[i].code;
    return 0;
}
int find_mot(const char* op){
    for(int i=0;i<MOTN;i++) if(strcmp(mot[i].op,op)==0) return i;
    return -1;
}
int sym_lookup(const char* name){
    for(int i=0;i<sc;i++) if(strcmp(sym[i].name,name)==0) return sym[i].addr;
    return -1;
}
void sym_insert(const char* name, int addr){
    if(sym_lookup(name)!=-1) return; /* ignore duplicate silently (short) */
    if(sc<MAX_SYM){ strcpy(sym[sc].name, name); sym[sc].addr = addr; sc++; }
}

/* Split at first comma (for R,OPR) */
void split_reg_mem(char *s, char *r, char *m){
    char *c = strchr(s, ',');
    if(c){ *c = '\0'; strcpy(r, s); strcpy(m, c+1); }
    else { r[0]='\0'; strcpy(m, s); }
    /* trim spaces */
    char *x=r; while(isspace((unsigned char)*x)) x++; memmove(r,x,strlen(x)+1);
    x=m; while(isspace((unsigned char)*x)) x++; memmove(m,x,strlen(x)+1);
    int len;
    len=(int)strlen(r); while(len>0 && isspace((unsigned char)r[len-1])) r[--len]='\0';
    len=(int)strlen(m); while(len>0 && isspace((unsigned char)m[len-1])) m[--len]='\0';
}

/* ---------- PASS 1 ---------- */
void pass1(const char *fname){
    FILE *fp = fopen(fname, "r");
    FILE *out = fopen("inter.txt", "w");
    FILE *stab = fopen("symtab.txt", "w");
    if(!fp||!out||!stab){ printf("File error!\n"); exit(0); }

    char line[MAX_LINE], t1[20], t2[20], t3[40];
    LC = 0; sc = 0;

    printf("\n--- PASS 1 ---\n");

    while(fgets(line, sizeof(line), fp)){
        if(isblankline(line)) continue;
        /* tokenize up to 3 simple fields */
        t1[0]=t2[0]=t3[0]='\0';
        int n = sscanf(line, "%19s %19s %39[^\n]", t1, t2, t3);
        if(n<=0) continue;

        if(strcmp(t1,"START")==0){
            START_ADDR = LC = atoi(t2);
            fprintf(out,"(AD,01)(C,%d)\n", LC);
            printf("(AD,01)(C,%d)\n", LC);
            continue;
        }
        if(strcmp(t1,"END")==0){
            fprintf(out,"(AD,02)\n");
            printf("(AD,02)\n");
            break;
        }

        /* DS/DC with label in t1 */
        if(strcmp(t2,"DS")==0){
            sym_insert(t1, LC);
            fprintf(out,"(DL,02)(C,%s)\n", t3);
            printf("(DL,02)(C,%s)\n", t3);
            LC += atoi(t3);
            continue;
        }
        if(strcmp(t2,"DC")==0){
            sym_insert(t1, LC);
            fprintf(out,"(DL,01)(C,%s)\n", t3);
            printf("(DL,01)(C,%s)\n", t3);
            LC += 1;
            continue;
        }

        /* Otherwise: Instruction line (label optional). We won’t store label unless DS/DC to keep it short. */
        /* Detect forms: 1 token (STOP), 2 tokens (READ X / PRINT X / op label?), 3 tokens (LBL OP OPR) */
        char op[20], opr[40];
        if(n==1){ /* single opcode like STOP */
            strcpy(op, t1); opr[0]='\0';
        } else if(n==2){
            /* Could be "OP OPR" */
            strcpy(op, t1); strcpy(opr, t2);
        } else { /* 3 tokens -> assume label, opcode, operand */
            strcpy(op, t2); strcpy(opr, t3);
        }

        int mi = find_mot(op);
        if(mi==-1){ /* unknown => treat as simple IS for brevity */
            fprintf(out,"(IS)(S)\n"); printf("(IS)(S)\n"); LC += 1; continue;
        }

        /* Record a compact IS line (we don’t expand operands here) */
        fprintf(out,"(IS,%02d)\n", mot[mi].code);
        printf("(IS,%02d)\n", mot[mi].code);
        LC += 1;
    }

    /* dump symbol table */
    for(int i=0;i<sc;i++)
        fprintf(stab,"%s %d\n", sym[i].name, sym[i].addr);

    fclose(fp); fclose(out); fclose(stab);

    printf("\nSymbol Table:\n");
    for(int i=0;i<sc;i++) printf("%s\t%d\n", sym[i].name, sym[i].addr);
    printf("\nPASS 1 completed. Generated inter.txt and symtab.txt\n");
}

/* ---------- PASS 2 ---------- */
void pass2(const char *fname){
    /* Load SYMTAB */
    FILE *stab = fopen("symtab.txt","r");
    if(!stab){ printf("symtab.txt not found. Run Pass 1 first.\n"); return; }
    sc = 0;
    while(fscanf(stab,"%19s %d", sym[sc].name, &sym[sc].addr)==2) sc++;
    fclose(stab);

    FILE *src = fopen(fname,"r");
    FILE *obj = fopen("object.txt","w");
    if(!src||!obj){ printf("File error in Pass 2!\n"); return; }

    char line[MAX_LINE], t1[20], t2[20], t3[40];
    LC = START_ADDR;

    printf("\n--- PASS 2 ---\n");

    while(fgets(line,sizeof(line),src)){
        if(isblankline(line)) continue;
        t1[0]=t2[0]=t3[0]='\0';
        int n = sscanf(line, "%19s %19s %39[^\n]", t1, t2, t3);
        if(n<=0) continue;

        if(strcmp(t1,"START")==0){ continue; }
        if(strcmp(t1,"END")==0){ break; }

        /* DS/DC with label in t1 */
        if(strcmp(t2,"DS")==0){
            /* No object code; reserve space -> just advance LC */
            int k = atoi(t3);
            fprintf(obj,"; %d DS %s\n", LC, t3);
            printf("; %d DS %s\n", LC, t3);
            LC += k;
            continue;
        }
        if(strcmp(t2,"DC")==0){
            int val = atoi(t3);
            /* Emit as: LC OPC REG ADDR  -> use opcode 00, reg 0, const */
            fprintf(obj,"%d 00 0 %d\n", LC, val);
            printf("%d 00 0 %d\n", LC, val);
            LC += 1;
            continue;
        }

        /* Instruction */
        char op[20], opr[40]; op[0]=opr[0]='\0';
        if(n==1){ strcpy(op,t1); }
        else if(n==2){ strcpy(op,t1); strcpy(opr,t2); }
        else { strcpy(op,t2); strcpy(opr,t3); }

        int mi = find_mot(op);
        if(mi==-1){ /* unknown, skip with comment */
            fprintf(obj,"; %d UNKNOWN %s\n", LC, op);
            printf("; %d UNKNOWN %s\n", LC, op);
            LC += 1;
            continue;
        }

        int rcode = 0, maddr = 0;
        if(mot[mi].needReg || mot[mi].needMem){
            char r[10], m[30]; r[0]=m[0]='\0';
            if(opr[0]) split_reg_mem(opr, r, m);
            if(mot[mi].needReg) rcode = regcode(r);
            if(mot[mi].needMem){
                if(m[0]=='\0'){ maddr=0; }
                else {
                    int a = sym_lookup(m);
                    if(a==-1) a = atoi(m); /* allow numeric literal fallback */
                    maddr = a;
                }
            }
        }

        fprintf(obj,"%d %02d %d %d\n", LC, mot[mi].code, rcode, maddr);
        printf("%d %02d %d %d\n", LC, mot[mi].code, rcode, maddr);
        LC += 1;
    }

    fclose(src); fclose(obj);
    printf("\nPASS 2 completed. Generated object.txt\n");
}

/* ---------- Viewer ---------- */
void show(const char *f){
    char x[160];
    FILE *p=fopen(f,"r");
    if(!p) return;
    while(fgets(x,sizeof(x),p)) printf("%s",x);
    fclose(p);
}

/* ---------- main ---------- */
int main(void){
    char fname[64];
    printf("Enter source file name: ");
    if(scanf("%63s", fname)!=1) return 0;

    printf("\nSource Code:\n");
    show(fname);

    pass1(fname);
    pass2(fname);

    printf("\n--- inter.txt ---\n"); show("inter.txt");
    printf("\n--- symtab.txt ---\n"); show("symtab.txt");
    printf("\n--- object.txt ---\n"); show("object.txt");
    return 0;
}
