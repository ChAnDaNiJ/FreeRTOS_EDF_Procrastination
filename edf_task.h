#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_TASKS 50
#define MAX_JOBS 1000

typedef struct {
    int taskId;
    int phase;
    int period;
    int wcet;
    int deadline;
} Task;

typedef struct {
    int taskId, jobId;
    int releaseTime, absDeadline;
    int remainingTime;
    int totalExecution;
    int firstStartTime;
    int completionTime;
    int lastExecTime;
    int infoIndex;
    int period;
} Job;

extern Task *taskSet;
extern int numTasks;

void getData(FILE *fp);
int computeHyperperiod(void);
double computeUtilization(void);

#endif