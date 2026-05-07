#include "pq.h"

void pqInit(PQ *pq){ pq->size=0; }
int pqEmpty(PQ *pq){ return pq->size==0; }

int higherEDF(Job a, Job b){
    if(a.absDeadline!=b.absDeadline)
        return a.absDeadline < b.absDeadline;
    return a.taskId < b.taskId;
}

void pqPushEDF(PQ *pq, Job j){
    int i=pq->size++;
    while(i>0){
        int p=(i-1)/2;
        if(higherEDF(pq->data[p],j)) break;
        pq->data[i]=pq->data[p]; i=p;
    }
    pq->data[i]=j;
}

Job pqPopEDF(PQ *pq){
    Job top=pq->data[0];
    Job last=pq->data[--pq->size];
    int i=0;
    while(2*i+1<pq->size){
        int c=2*i+1;
        if(c+1<pq->size && higherEDF(pq->data[c+1],pq->data[c])) c++;
        if(higherEDF(last,pq->data[c])) break;
        pq->data[i]=pq->data[c]; i=c;
    }
    pq->data[i]=last;
    return top;
}

void pqPushRM(PQ *pq, Job j){
    int i=pq->size++;
    while(i>0){
        int p=(i-1)/2;
        if(pq->data[p].period <= j.period) break;
        pq->data[i]=pq->data[p]; i=p;
    }
    pq->data[i]=j;
}

Job pqPopRM(PQ *pq){
    Job top=pq->data[0];
    Job last=pq->data[--pq->size];
    int i=0;
    while(2*i+1<pq->size){
        int c=2*i+1;
        if(c+1<pq->size && pq->data[c+1].period<pq->data[c].period) c++;
        if(last.period<=pq->data[c].period) break;
        pq->data[i]=pq->data[c]; i=c;
    }
    pq->data[i]=last;
    return top;
}