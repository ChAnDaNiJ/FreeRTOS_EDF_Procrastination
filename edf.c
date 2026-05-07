#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "pq.h"
#include "metrics.h"
#include "procrastination.h"

extern int useActual;


#define SHUTDOWN_THRESHOLD 15


static int findNextArrival(int currentTime, int hyperperiod)
{
    int next = hyperperiod;
    for (int i = 0; i < numTasks; i++)
    {
        int p          = taskSet[i].period;
        int nextRelease = ((currentTime / p) + 1) * p;
        if (nextRelease < next)
            next = nextRelease;
    }
    return next;
}


static int findNextArrivalInWindow(int currentTime, int limit)
{
    int next = limit + 1;
    for (int i = 0; i < numTasks; i++)
    {
        int p          = taskSet[i].period;
        int nextRelease = ((currentTime / p) + 1) * p;
        if (nextRelease <= limit && nextRelease < next)
            next = nextRelease;
    }
    return next;
}


static int releaseJobs(PQ *pq, int time, int jobId[])
{
    int released = 0;
    for (int i = 0; i < numTasks; i++)
    {
        if (time % taskSet[i].period == 0)
        {
            Job j;
            j.taskId      = i + 1;
            j.jobId       = ++jobId[i];
            j.releaseTime = time;
            j.absDeadline = time + taskSet[i].deadline;
            j.period      = taskSet[i].period;

            int wcet = taskSet[i].wcet;

            if (useActual)
            {
                int minExec     = (int)ceil(0.4 * wcet);
                int maxExec     = wcet;
                j.remainingTime = rand() % (maxExec - minExec + 1) + minExec;
                printf(" T%d J%d released at %d  exec=%d  deadline=%d\n",
                       j.taskId, j.jobId, time,
                       j.remainingTime, j.absDeadline);
            }
            else
            {
                j.remainingTime = wcet;
            }

            j.totalExecution = 0;
            j.firstStartTime = -1;
            j.lastExecTime   = -1;
            j.infoIndex      = infoCount;

            info[infoCount++] = (JobInfo){
                j.taskId, j.jobId,
                time, 0, -1,
                j.absDeadline, 0
            };

            pqPushEDF(pq, j);
            released++;
        }
    }
    return released;
}


static int checkDeadlines(PQ *pq, int time)
{
    for (int i = 0; i < pq->size; i++)
    {
        if (pq->data[i].remainingTime > 0 &&
            time >= pq->data[i].absDeadline)
        {
            printf("\n DEADLINE MISSED: T%d J%d"
                   " at time %d (deadline=%d)\n",
                   pq->data[i].taskId,
                   pq->data[i].jobId,
                   time,
                   pq->data[i].absDeadline);
            return 1;
        }
    }
    return 0;
}


void edfScheduler(int hyperperiod)
{
    PQ  pq;
    pqInit(&pq);

    int time             = 0;
    int jobId[MAX_TASKS] = {0};

    releaseJobs(&pq, time, jobId);

    while (time < hyperperiod)
    {
        if (checkDeadlines(&pq, time))
            return;


        if (pqEmpty(&pq))
        {
            int NSI = calculateNSI(taskSet, numTasks,
                                   time, SHUTDOWN_THRESHOLD);

            if (NSI > time && NSI <= hyperperiod)
            {
   
                printf(" SHUTDOWN  %d -> %d  (duration=%d)\n",
                       time, NSI, NSI - time);

                sched[schedSize++] = (Schedule){
                    time, NSI, -2, -1, 1
                };

                int shutdownStart = sched[schedSize - 1].start;
                time = NSI;


                for (int t = shutdownStart + 1; t <= NSI; t++)
                    releaseJobs(&pq, t, jobId);
            }
            else
            {

                int nextArrival = findNextArrival(time, hyperperiod);
                if (nextArrival > hyperperiod)
                    nextArrival = hyperperiod;

                printf(" IDLE      %d -> %d  (duration=%d)\n",
                       time, nextArrival, nextArrival - time);

                sched[schedSize++] = (Schedule){
                    time, nextArrival, -1, -1, 1
                };

                time = nextArrival;

                releaseJobs(&pq, time, jobId);
            }

            continue;
        }

   
        Job j = pqPopEDF(&pq);

        if (j.firstStartTime == -1)
        {
            j.firstStartTime = time;
            info[j.infoIndex].firstStartTime = time;
        }

        int jobFinish = time + j.remainingTime;

       
        int nextArr = findNextArrivalInWindow(time, jobFinish - 1);

        int runUntil;
        if (nextArr <= jobFinish - 1)
            runUntil = nextArr;     
        else
            runUntil = jobFinish;    

        int ran = runUntil - time;

        sched[schedSize++] = (Schedule){
            time, runUntil, j.taskId, j.jobId, 0
        };

        j.remainingTime  -= ran;
        j.totalExecution += ran;
        j.lastExecTime    = runUntil - 1;
        info[j.infoIndex].totalExecution += ran;

        time = runUntil;

        if (j.remainingTime == 0)
        {
            sched[schedSize - 1].isComplete = 1;
            info[j.infoIndex].completionTime = time;
            printf(" T%d J%d  DONE  at %d\n",
                   j.taskId, j.jobId, time);

            releaseJobs(&pq, time, jobId);
        }
        else
        {
        
            pqPushEDF(&pq, j);

           
            releaseJobs(&pq, time, jobId);

            printf(" Preemption point at %d"
                   " (T%d J%d remaining=%d)\n",
                   time, j.taskId, j.jobId, j.remainingTime);
        }

    }
}
