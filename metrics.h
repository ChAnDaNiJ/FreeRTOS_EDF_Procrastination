#ifndef METRICS_H
#define METRICS_H

#include "edf_task.h"

typedef struct{
    int start,end,taskId,jobId,isComplete;
}Schedule;

typedef struct{
    int taskId,jobId;
    int releaseTime,completionTime,firstStartTime;
    int absDeadline,totalExecution;
}JobInfo;

extern Schedule sched[MAX_JOBS];
extern int schedSize;
extern JobInfo info[MAX_JOBS];
extern int infoCount;

void printSchedule();
void printMetrics();

#endif