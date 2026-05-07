#include "procrastination.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "edf_task.h"

int compareJobs(const void* a, const void* b) {
    JobInstance* jobA = (JobInstance*)a;
    JobInstance* jobB = (JobInstance*)b;
    return (jobB->absDeadline - jobA->absDeadline);
}

int getEarliestJob(Task* tasks, int numTasks, 
                   int currentTime, JobInstance* earliest) {
    int minDeadline = INT_MAX;
    int hyperperiod = computeHyperperiod();
    int found = 0;

    for (int i = currentTime; i < hyperperiod; i++) {
        for (int j = 0; j < numTasks; j++) {
            if (i % tasks[j].period == 0) {
                int deadline = i + tasks[j].period;
                if (deadline < minDeadline) {
                    minDeadline = deadline;
                    earliest->taskId = j;
                    earliest->releaseTime = i;
                    earliest->absDeadline = deadline;
                    earliest->wcet = tasks[j].wcet;
                    earliest->utilization = 
                        (float)tasks[j].wcet / tasks[j].period;
                    found = 1;
                }
            }
        }
    }
    return found;
}

int getDnext2(Task* tasks, int numTasks, 
              int currentTime, int Dnext1) {
    
    int lowestPriorityDeadline = -1; 
    
    for (int i = currentTime; i <= Dnext1; i++) { 
        for (int j = 0; j < numTasks; j++) {
            if (i % tasks[j].period == 0) {
                int deadline = i + tasks[j].period;
                if (deadline > lowestPriorityDeadline) {
                    lowestPriorityDeadline = deadline;
                }
            }
        }
    }
    
    if (lowestPriorityDeadline == -1)
        lowestPriorityDeadline = Dnext1;
    
    return lowestPriorityDeadline;
}


int collectJobs(Task* tasks, int numTasks, int currentTime,
                int Dnext2, JobInstance** listOut) {

    int count = 0;
    for (int i = 0; i < numTasks; i++) {
        int firstRelease = ((currentTime + tasks[i].period - 1) / tasks[i].period) * tasks[i].period;
        for (int r = firstRelease; r <= Dnext2; r += tasks[i].period) {
            count++;
        }
    }

    if (count == 0) {
        *listOut = NULL;
        return 0;
    }

   
    JobInstance* list = (JobInstance*) malloc(sizeof(JobInstance) * count);
    if (!list) {
        *listOut = NULL;
        return -1; 
    }

    
    int idx = 0;
    for (int i = 0; i < numTasks; i++) {
        int firstRelease = ((currentTime + tasks[i].period - 1)
                            / tasks[i].period) * tasks[i].period;

        for (int r = firstRelease; r <= Dnext2; r += tasks[i].period) {
            list[idx].taskId      = i;
            list[idx].jobId       = r / tasks[i].period; // FIX 3: jobId set correctly
            list[idx].releaseTime = r;
            list[idx].absDeadline = r + tasks[i].period;
            list[idx].wcet        = tasks[i].wcet;
            list[idx].utilization = (float)tasks[i].wcet / tasks[i].period;
            idx++;
        }
    }

    *listOut = list;
    return count;
}



int calculateNSI(Task* tasks, int numTasks, int currentTime, int threshold) {

    JobInstance earliestJob;
    if (!getEarliestJob(tasks, numTasks, currentTime, &earliestJob)) {
        return -1; // No future jobs found (end of hyperperiod)
    }

    int Dnext1 = earliestJob.absDeadline;

    
    if ((Dnext1 - currentTime - earliestJob.wcet) < threshold) {
        return -1;
    }

    int Dnext2 = getDnext2(tasks, numTasks, currentTime, Dnext1);


    JobInstance* list = NULL;
    int K = collectJobs(tasks, numTasks, currentTime, Dnext2, &list);

    if (K <= 0 || list == NULL) {
        return -1;
    }

    // Sort: non-increasing deadline order (paper Step 4)
    qsort(list, K, sizeof(JobInstance), compareJobs);

   
    double tempNSI = (double)Dnext2;

    double Up = 0.0;
    for (int i = 0; i < numTasks; i++) {
        Up += (double)tasks[i].wcet / tasks[i].period;
    }

  
    for (int i = 0; i < K; i++) {

        if (list[i].absDeadline > Dnext2) {

            double overlap = (double)(Dnext2 - list[i].releaseTime);
            tempNSI -= (overlap * list[i].utilization * Up);
        } else {
           
            tempNSI -= (double)list[i].wcet;
        }

       
        if (i < K - 1 && tempNSI > (double)list[i + 1].absDeadline) {
            tempNSI = (double)list[i + 1].absDeadline;
        }
    }

    int NSI = (int)tempNSI;
    free(list);

   
    if ((NSI - currentTime) < threshold) {
        return -1;
    }

    
    return NSI;
}