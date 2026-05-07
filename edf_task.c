#include "edf_task.h"

Task *taskSet;
int numTasks = 0;

static int gcd(int a, int b){ return b==0?a:gcd(b,a%b); }
static int lcm(int a, int b){ return (a*b)/gcd(a,b); }

void getData(FILE *fp){
    fscanf(fp,"%d",&numTasks);
    if(numTasks<=0){ printf("Tasks must be >0\n"); exit(1); }

    taskSet = malloc(sizeof(Task)*numTasks);

    for(int i=0;i<numTasks;i++){
        fscanf(fp,"%d %d %d %d",
        &taskSet[i].phase,
        &taskSet[i].period,
        &taskSet[i].wcet,
        &taskSet[i].deadline);
        taskSet[i].taskId=i+1;
    }
}

int computeHyperperiod(void){
    int hp=taskSet[0].period;
    for(int i=1;i<numTasks;i++)
        hp=lcm(hp,taskSet[i].period);
    return hp;
}

double computeUtilization(void){
    double u=0;
    for(int i=0;i<numTasks;i++)
        u += (double)taskSet[i].wcet/taskSet[i].period;
    return u;
}