#include <stdio.h>
#include <math.h>
#include "metrics.h"

Schedule sched[MAX_JOBS];
int schedSize = 0;

JobInfo info[MAX_JOBS];
int infoCount = 0;

void printSchedule()
{
    printf("\nStart   End     Task    Job\n");
    printf("-----   -----   -----   -----\n");

    if (schedSize == 0) return;

    int start = sched[0].start;
    int end   = sched[0].end;
    int task  = sched[0].taskId;
    int job   = sched[0].jobId;

    for (int i = 1; i < schedSize; i++)
    {
        if (sched[i].taskId == task && sched[i].jobId == job)
        {
            end = sched[i].end;
        }
        else
        {
            if (task == -1)
                printf("%-7d %-7d IDLE\n", start, end);
            else if (task == -2)
                printf("%-7d %-7d SHUTDOWN\n", start, end);
            else
                printf("%-7d %-7d T%-6d J%-6d\n", start, end, task, job);

            start = sched[i].start;
            end   = sched[i].end;
            task  = sched[i].taskId;
            job   = sched[i].jobId;
        }
    }

    if (task == -1)
        printf("%-7d %-7d IDLE\n", start, end);
    else if (task == -2)
        printf("%-7d %-7d SHUTDOWN\n", start, end);
    else
        printf("%-7d %-7d T%-6d J%-6d\n", start, end, task, job);
}

void printMetrics()
{
    FILE *fp = fopen("output.txt", "w");

    fprintf(fp, "Decision Points: %d\n", schedSize);

    int cs = 0, vol = 0, invol = 0, pre = 0;
    for (int i = 1; i < schedSize; i++)
    {
        int prevTask = sched[i-1].taskId;
        int currTask = sched[i].taskId;

        if (prevTask >= 1 && currTask >= 1 && currTask != prevTask)
        {
            cs++;
            if (sched[i-1].isComplete) vol++;
            else { invol++; pre++; }
        }
    }
    fprintf(fp, "Context Switches:%d Vol:%d Invol:%d\n", cs, vol, invol);
    fprintf(fp, "Preemptions:%d\n", pre);

    int cache = 0, last = -1;
    for (int i = 0; i < schedSize; i++)
    {
        int t = sched[i].taskId;
        if (t < 1) continue;          
        if (last != -1 && t != last) cache++;
        last = t;
    }
    fprintf(fp, "Cache Impact:%d\n", cache);


    int total = sched[schedSize-1].end;
    int nonActive = 0;
    for (int i = 0; i < schedSize; i++)
    {
        if (sched[i].taskId < 1)   
            nonActive += sched[i].end - sched[i].start;
    }
    fprintf(fp, "CPU Util:%.3f\n", (double)(total - nonActive) / total);

    int    minRT[50],  maxRT[50];
    int    minWT[50],  maxWT[50];
    int    minLat[50], maxLat[50];
    int    minLate[50],maxLate[50];
    double sumRT[50]  = {0}, sumWT[50]  = {0};
    double sumLat[50] = {0}, sumLate[50]= {0};
    int    cnt[50]    = {0};

    for (int i = 0; i < 50; i++)
    {
        minRT[i] = minWT[i] = minLat[i] = 1000000;
        maxRT[i] = maxWT[i] = maxLat[i] = 0;
        minLate[i] =  1000000;
        maxLate[i] = -1000000;
    }

    for (int i = 0; i < infoCount; i++)
    {
        int t    = info[i].taskId - 1;
        int rt   = info[i].completionTime - info[i].releaseTime;
        int wt   = rt - info[i].totalExecution;
        int lat  = info[i].completionTime - info[i].firstStartTime;
        int late = info[i].completionTime - info[i].absDeadline;

        if (rt   < minRT[t])   minRT[t]   = rt;
        if (rt   > maxRT[t])   maxRT[t]   = rt;
        if (wt   < minWT[t])   minWT[t]   = wt;
        if (wt   > maxWT[t])   maxWT[t]   = wt;
        if (lat  < minLat[t])  minLat[t]  = lat;
        if (lat  > maxLat[t])  maxLat[t]  = lat;
        if (late < minLate[t]) minLate[t] = late;
        if (late > maxLate[t]) maxLate[t] = late;

        sumRT[t]   += rt;
        sumWT[t]   += wt;
        sumLat[t]  += lat;
        sumLate[t] += late;
        cnt[t]++;
    }

    for (int i = 0; i < 50; i++)
    {
        if (cnt[i] == 0) continue;
        fprintf(fp, "\nTask %d\n", i + 1);
        fprintf(fp, "RT   min:%d max:%d avg:%.2f\n",minRT[i],   maxRT[i],   sumRT[i]   / cnt[i]);
        fprintf(fp, "WT   min:%d max:%d avg:%.2f\n",minWT[i],   maxWT[i],   sumWT[i]   / cnt[i]);
        fprintf(fp, "Latency  min:%d max:%d avg:%.2f\n",minLat[i],  maxLat[i],  sumLat[i]  / cnt[i]);
        fprintf(fp, "Lateness min:%d max:%d avg:%.2f\n",minLate[i], maxLate[i], sumLate[i] / cnt[i]);
    }

    fclose(fp);
}
