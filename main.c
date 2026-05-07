#include <stdio.h>
#include <stdlib.h>
#include "edf_task.h"

#include "FreeRTOS.h"
#include "task.h"

extern void schedulerTask(void *);

int useActual = 1;

int main()
{
    FILE *fp = fopen("../file1.txt", "r");

    if (!fp)
    {
        printf("File error\n");
        return 1;
    }

    getData(fp);
    fclose(fp);

    xTaskCreate(
        schedulerTask,
        "EDF",
        2048,
        NULL,
        2,
        NULL
    );

    vTaskStartScheduler();

    return 0;
}
