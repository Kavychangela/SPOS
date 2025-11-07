#include <stdio.h>
#include <limits.h>

struct Hole { int size, start, free; } h[10], temp[10];
struct Proc { int size, alloc, hid, addr; } p[10];
int nh, np, last = 0;

void copy() { for (int i=0;i<nh;i++) temp[i]=h[i]; }
void report() {
    printf("\nProc\tSize\tHole\tAddr\n");
    for (int i=0;i<np;i++)
        if (p[i].alloc) printf("P%d\t%d\tH%d\t%d\n", i+1,p[i].size,p[i].hid,p[i].addr);
        else printf("P%d\t%d\tNot Allocated\n", i+1,p[i].size);
}

void first() {
    for(int i=0;i<np;i++) for(int j=0;j<nh;j++) if(temp[j].free&&temp[j].size>=p[i].size){
        p[i].alloc=1; p[i].hid=j; p[i].addr=temp[j].start;
        temp[j].start+=p[i].size; temp[j].size-=p[i].size;
        if(temp[j].size==0) temp[j].free=0; break;
    }
}

void best() {
    for(int i=0;i<np;i++){int best=-1,bs=INT_MAX;
        for(int j=0;j<nh;j++) if(temp[j].free&&temp[j].size>=p[i].size&&temp[j].size<bs)
            {bs=temp[j].size; best=j;}
        if(best!=-1){p[i].alloc=1;p[i].hid=best;p[i].addr=temp[best].start;
            temp[best].start+=p[i].size; temp[best].size-=p[i].size;
            if(temp[best].size==0) temp[best].free=0;}
    }
}

void worst() {
    for(int i=0;i<np;i++){int worst=-1,ws=-1;
        for(int j=0;j<nh;j++) if(temp[j].free&&temp[j].size>=p[i].size&&temp[j].size>ws)
            {ws=temp[j].size; worst=j;}
        if(worst!=-1){p[i].alloc=1;p[i].hid=worst;p[i].addr=temp[worst].start;
            temp[worst].start+=p[i].size; temp[worst].size-=p[i].size;
            if(temp[worst].size==0) temp[worst].free=0;}
    }
}

void next() {
    for(int i=0;i<np;i++){int j=last,c=0;
        while(c<nh){if(temp[j].free&&temp[j].size>=p[i].size){
            p[i].alloc=1;p[i].hid=j;p[i].addr=temp[j].start;
            temp[j].start+=p[i].size; temp[j].size-=p[i].size;
            if(temp[j].size==0) temp[j].free=0; last=j; break;}
            j=(j+1)%nh; c++; }
    }
}

int main(){
    printf("No. of holes: "); scanf("%d",&nh);
    for(int i=0;i<nh;i++){printf("Hole %d size,start: ",i); scanf("%d%d",&h[i].size,&h[i].start); h[i].free=1;}
    printf("No. of processes: "); scanf("%d",&np);
    for(int i=0;i<np;i++){printf("Size of P%d: ",i+1); scanf("%d",&p[i].size); p[i].alloc=0;}
    while(1){
        int ch; printf("\n1.First 2.Best 3.Worst 4.Next 5.Exit: "); scanf("%d",&ch);
        if(ch==5) break;
        copy();
        for(int i=0;i<np;i++){p[i].alloc=0;}
        switch(ch){case 1:first();break;case 2:best();break;case 3:worst();break;case 4:next();break;}
        report();
    }
}