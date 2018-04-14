/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32140033 32163322
*	    Student name : 강민구   이승현
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
#include <math.h>
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
#include <stdbool.h>
#include "lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */

enum scheduler { FIFO = 1, SPN, R_R, MLFQ, LOTTERY, QUIT };

int main(int argc, char *argv[]){
	bool **workLoad = NULL;
	bool flag = true;
	int totalServiceTime = 0;
	int selectMenu;
	Process processSet[PROCESS_COUNT] = {
		//도착시작, 총 수행시간, 현재 수행시간, 축적시간, 프로세스 아이디, 티켓, 우선순위
		{ 0, 2, 0, 0 ,0, 10, 0 },
		{ 0, 7, 0, 0, 1, 30, 0 },
		{ 5, 10, 0, 0, 2, 20, 0 },
		{ 7, 3, 0, 0, 3, 50, 0 },
		{ 9, 3, 0, 0, 4, 70, 0 },
	};

	for(int i = 0; i < 5; i++){
		totalServiceTime += processSet[i].serviceTime;	
	}

	workLoad = createWorkLoad(totalServiceTime, workLoad);

	while(flag){
		showMenu();
		printf("Select menu : ");
		scanf("%d", &selectMenu);
		initComponent(processSet, workLoad, totalServiceTime);
		switch(selectMenu){
			case FIFO :
				firstInFirstOut(processSet, workLoad, totalServiceTime);
				showWorkLoad(totalServiceTime, workLoad);
				break;
			case SPN :
				SJF(processSet, totalServiceTime, workLoad);
				showWorkLoad(totalServiceTime, workLoad);
				break;
			case R_R :
				RR(processSet, totalServiceTime, workLoad, 2);
				showWorkLoad(totalServiceTime, workLoad);
				break;
			case MLFQ :
				multilevelFeedbackQueue(processSet, workLoad, totalServiceTime, 2, false);
				showWorkLoad(totalServiceTime, workLoad);
				break;
			case LOTTERY :
				Lottery(processSet, totalServiceTime, workLoad);
				showWorkLoad(totalServiceTime, workLoad);
				break;
			case QUIT :
				flag = false;
				break;
			default :
				printf("Wrong number\n");
				break;
		}
	}
}
