#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MNT { char name[20]; int mdt; } mnt[10];
char mdt[50][80];
int mntc=0, mdtc=0;

int main() {
    FILE *src=fopen("source.txt","r"), *inter=fopen("inter.txt","w");
    if(!src){printf("Source missing!");return 0;}
    char w1[20], w2[20], w3[20];
    while(fscanf(src,"%s",w1)!=EOF){
        if(!strcmp(w1,"MACRO")){
            fscanf(src,"%s%s",w1,w2);
            strcpy(mnt[mntc].name,w1);
            mnt[mntc++].mdt=mdtc;
            fprintf(stdout,"Macro: %s\n",w1);
            while(fscanf(src,"%s",w1)!=EOF && strcmp(w1,"MEND")){
                strcpy(mdt[mdtc++],w1);
            }
            strcpy(mdt[mdtc++],"MEND");
        }else fprintf(inter,"%s\n",w1);
    }
    fclose(src); fclose(inter);
    FILE *f1=fopen("MNT.txt","w"), *f2=fopen("MDT.txt","w");
    for(int i=0;i<mntc;i++) fprintf(f1,"%s %d\n",mnt[i].name,mnt[i].mdt);
    for(int i=0;i<mdtc;i++) fprintf(f2,"%s\n",mdt[i]);
    fclose(f1); fclose(f2);
    printf("\nPASS 1 done (MNT, MDT, inter.txt created)\n");
}
