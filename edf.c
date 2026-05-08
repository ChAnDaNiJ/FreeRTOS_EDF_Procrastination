#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "pq.h"
#include "metrics.h"
#include "procrastination.h"

extern int useActual;

#define SHUTDOWN_THRESHOLD 15

/* --------------------------------------------------- */
/* Find next arrival time                              */
/* --------------------------------------------------- */

static int findNextArrival(int currentTime, int hyperperiod)
{
    int next = hyperperiod;

    for (int i = 0; i < numTasks; i++)
    {
        int p = taskSet[i].period;

        int nextRelease =
            ((currentTime / p) + 1) * p;

        if (nextRelease < next)
            next = nextRelease;
    }

    return next;
}

/* --------------------------------------------------- */
/* Release jobs at current tick                        */
/* --------------------------------------------------- */

static void releaseJobs(PQ *pq, int time, int jobId[])
{
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
                int minExec = (int)ceil(0.4 * wcet);
                int maxExec = wcet;

                j.remainingTime =
                    rand() % (maxExec - minExec + 1)
                    + minExec;
            }
            else
            {
                j.remainingTime = wcet;
            }

            j.totalExecution = 0;
            j.firstStartTime = -1;
            j.lastExecTime   = -1;
            j.infoIndex      = infoCount;

            info[infoCount++] = (JobInfo)
            {
                j.taskId,
                j.jobId,
                time,
                0,
                -1,
                j.absDeadline,
                0
            };

            pqPushEDF(pq, j);

            printf(" T%d J%d released at %d  exec=%d  deadline=%d\n",
                   j.taskId,
                   j.jobId,
                   time,
                   j.remainingTime,
                   j.absDeadline);
        }
    }
}

/* --------------------------------------------------- */
/* Deadline checking                                   */
/* --------------------------------------------------- */

static int checkDeadlines(PQ *pq, int time)
{
    for (int i = 0; i < pq->size; i++)
    {
        if (pq->data[i].remainingTime > 0 &&
            time >= pq->data[i].absDeadline)
        {
            printf("\n DEADLINE MISSED: "
                   "T%d J%d at time %d "
                   "(deadline=%d)\n",
                   pq->data[i].taskId,
                   pq->data[i].jobId,
                   time,
                   pq->data[i].absDeadline);

            return 1;
        }
    }

    return 0;
}

/* --------------------------------------------------- */
/* EDF Scheduler                                       */
/* --------------------------------------------------- */

void edfScheduler(int hyperperiod)
{
    PQ pq;
    pqInit(&pq);

    int time = 0;

    int jobId[MAX_TASKS] = {0};

    Job currentJob;
    int hasRunningJob = 0;

    printf("EDF Scheduler Started\n");

    while (time <= hyperperiod)
    {
        printf("\n[Scheduler] Tick: %d\n", time);

        /* ---------------------------------------- */
        /* Release periodic jobs                    */
        /* ---------------------------------------- */

        releaseJobs(&pq, time, jobId);

        /* ---------------------------------------- */
        /* Deadline check                           */
        /* ---------------------------------------- */

        if (checkDeadlines(&pq, time))
            return;

        /* ---------------------------------------- */
        /* If no running job, pick next EDF job     */
        /* ---------------------------------------- */

        if (!hasRunningJob)
        {
            if (!pqEmpty(&pq))
            {
                currentJob = pqPopEDF(&pq);

                hasRunningJob = 1;

                if (currentJob.firstStartTime == -1)
                {
                    currentJob.firstStartTime = time;

                    info[currentJob.infoIndex]
                        .firstStartTime = time;
                }

                printf(" -> Running T%d J%d\n",
                       currentJob.taskId,
                       currentJob.jobId);
            }
        }

        /* ---------------------------------------- */
        /* Execute 1 tick                           */
        /* ---------------------------------------- */

        if (hasRunningJob)
        {
            currentJob.remainingTime--;

            currentJob.totalExecution++;

            info[currentJob.infoIndex]
                .totalExecution++;

            /* -------------------------------- */
            /* Job completed                    */
            /* -------------------------------- */

            if (currentJob.remainingTime == 0)
            {
                info[currentJob.infoIndex]
                    .completionTime = time + 1;

                printf(" T%d J%d DONE at %d\n",
                       currentJob.taskId,
                       currentJob.jobId,
                       time + 1);

                hasRunningJob = 0;
            }
            else
            {
                /* ---------------------------- */
                /* Check for preemption         */
                /* ---------------------------- */

                if (!pqEmpty(&pq))
                {
                    Job top = pq.data[0];

                    if (
                        top.absDeadline < currentJob.absDeadline ||
                    
                        (
                            top.absDeadline ==
                            currentJob.absDeadline &&
                    
                            top.taskId <
                            currentJob.taskId
                        )
                    )
                    {
                        printf(" Preemption point at %d "
                               "(T%d J%d remaining=%d)\n",
                               time,
                               currentJob.taskId,
                               currentJob.jobId,
                               currentJob.remainingTime);

                        pqPushEDF(&pq, currentJob);

                        currentJob = pqPopEDF(&pq);

                        if (currentJob.firstStartTime == -1)
                        {
                            currentJob.firstStartTime = time + 1;

                            info[currentJob.infoIndex]
                                .firstStartTime = time + 1;
                        }

                        printf(" -> Running T%d J%d\n",
                               currentJob.taskId,
                               currentJob.jobId);
                    }
                }
            }
        }
        else
        {
            /* ------------------------------------ */
            /* Idle / Shutdown handling             */
            /* ------------------------------------ */

            if (pqEmpty(&pq))
            {
                int NSI =
                    calculateNSI(taskSet,
                                 numTasks,
                                 time,
                                 SHUTDOWN_THRESHOLD);

                if (NSI > time &&
                    NSI <= hyperperiod)
                {
                    printf(" SHUTDOWN  %d -> %d "
                           "(duration=%d)\n",
                           time,
                           NSI,
                           NSI - time);

                    sched[schedSize++] =
                    (Schedule)
                    {
                        time,
                        NSI,
                        -2,
                        -1,
                        1
                    };

                    int shutdownStart = time;

                    time = NSI;

                    /* ---------------------------- */
                    /* IMPORTANT FIX                */
                    /* Release jobs during sleep    */
                    /* ---------------------------- */

                    for (int t = shutdownStart + 1;
                         t <= NSI;
                         t++)
                    {
                        releaseJobs(&pq, t, jobId);
                    }

                    continue;
                }
                else
                {
                    printf(" IDLE\n");
                }
            }
        }

        time++;
    }

    printf("\nEDF execution completed\n");
}
