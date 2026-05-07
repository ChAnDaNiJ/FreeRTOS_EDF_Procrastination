#ifndef PROCRASTINATION_H
#define PROCRASTINATION_H

#include "edf_task.h"
#include <limits.h>


typedef struct {
    int   taskId;       
    int   jobId;        
    int   releaseTime;  
    int   absDeadline;  
    int   wcet;        
    float utilization;  
} JobInstance;


int compareJobs(const void* a, const void* b);
int getEarliestJob(Task* tasks,int numTasks,int currentTime,JobInstance* earliest);
int getDnext2(Task* tasks,int numTasks,int currentTime,int Dnext1);
int collectJobs(Task* tasks,int numTasks,int currentTime, int Dnext2, JobInstance** listOut);
int calculateNSI(Task* tasks, int numTasks, int   currentTime, int   threshold);


#endif