/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */

int main(int argc, char *argv[]){
	//프로세스 생성
	Process processSet[5];
	int totalRunningTime = 0;
	bool **workload;
	processSet[0].arriveTime = 0;
    	processSet[0].serviceTime = 5;
    	processSet[0].currentServiceTime = 0;
    	processSet[0].processId = 0;
    	

    	processSet[1].arriveTime = 0;
    	processSet[1].serviceTime = 2;
    	processSet[1].currentServiceTime = 0;
    	processSet[1].processId = 1;
    //	processSet[1].accumulatedTime = 0;

    	processSet[2].arriveTime = 4;
    	processSet[2].serviceTime = 4;
    	processSet[2].currentServiceTime = 0;
    	processSet[2].processId = 2;
    //	processSet[2].accumulatedTime = 0;

    	processSet[3].arriveTime = 6;
    	processSet[3].serviceTime = 5;
    	processSet[3].currentServiceTime = 0;
    	processSet[3].processId = 3;
    //	processSet[3].accumulatedTime = 0;

    	processSet[4].arriveTime = 8;
    	processSet[4].serviceTime = 2;
    	processSet[4].currentServiceTime = 0;
    	processSet[4].processId = 4;
    //	processSet[4].accumulatedTime = 0;
	

    	for(int i = 0; i < 5; i++){
        	totalRunningTime += processSet[i].serviceTime;    
    	}
	workload = createWorkLoad(5, totalRunningTime, workload);

	for(int i=0;i<5;i++){
		for(int j=0;j<totalRunningTime;j++){
			workload[i][j]=false;
		}
	}


	SJF(processSet, 5, totalRunningTime, workload);
	showWorkLoad(5, totalRunningTime, workload);

	return 0;
}

