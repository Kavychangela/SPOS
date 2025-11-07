#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MNT { char name[20]; int mdt; } mnt[10];
char mdt[50][80];
int mntc=0, mdtc=0;

int find(char *x){for(int i=0;i<mntc;i++)if(!strcmp(x,mnt[i].name))return i;return -1;}

int main(){
    FILE *f1=fopen("MNT.txt","r"),*f2=fopen("MDT.txt","r"),*f3=fopen("inter.txt","r");
    if(!f1||!f2||!f3){printf("Run Pass1 first!");return 0;}
    while(fscanf(f1,"%s%d",mnt[mntc].name,&mnt[mntc].mdt)==2)mntc++;
    while(fscanf(f2,"%s",mdt[mdtc])==1)mdtc++;
    char w[20];
    printf("\n--- PASS 2 OUTPUT ---\n");
    while(fscanf(f3,"%s",w)!=EOF){
        int k=find(w);
        if(k==-1) printf("%s\n",w);
        else for(int i=mnt[k].mdt;i<mdtc && strcmp(mdt[i],"MEND");i++) printf("%s\n",mdt[i]);
    }
    fclose(f1);fclose(f2);fclose(f3);
    printf("\nPASS 2 done (macros expanded)\n");
}
