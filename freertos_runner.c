#include <stdio.h>

#include "edf_task.h"
#include "FreeRTOS.h"
#include "task.h"

extern void edfScheduler(int);

void schedulerTask(void *pvParameters)
{
    int hp = computeHyperperiod();

    printf("FreeRTOS started EDF Scheduler\n");

    edfScheduler(hp);

    printf("\nEDF execution completed\n");

    while(1);
}
