#ifndef PQ_H
#define PQ_H

#include "edf_task.h"

typedef struct {
    Job data[MAX_JOBS];
    int size;
} PQ;

void pqInit(PQ *pq);
void pqPushEDF(PQ *pq, Job j);
Job pqPopEDF(PQ *pq);
void pqPushRM(PQ *pq, Job j);
Job pqPopRM(PQ *pq);
int pqEmpty(PQ *pq);

#endif