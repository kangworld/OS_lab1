/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32140033
*	    Student name : 강민구
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
 * you need to implement FCFS, RR, SPN, MLFQ Lottery scheduler. 
 */
void initQue(Queue *queue){
	queue->front = queue->rear = NULL;
	queue->count = 0;
}

bool isEmpty(Queue *queue){
	if(queue->count == 0){
		return true;
	}else{
		return false;
	}
}

/*
   MLFQ에서 모든 큐가 공백인지 검사하는 함수
 */
bool isAllEmpty(Queue *multiQueue, int queueLevel){
	for(int i = 0; i < queueLevel; i++){
		if(isEmpty(&multiQueue[i]) == false){
			return false;
		}
	}
	return true;
}

void enQueueRear(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
	}else{
		queue->rear->next = newNode;
	}
	queue->rear = newNode;
	queue->count++;
}

void enQueueFront(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
	}else{
		newNode->next = queue->front;
		queue->front = newNode;
	}
	queue->count++;
}

Process * deQueue(Queue *queue){
	Process *delProc;
	Node *curNode;

	if(isEmpty(queue)){
		return NULL;
	}else{
		curNode = queue->front;
		delProc = curNode->process;
		queue->front = curNode->next;
		free(curNode);
		queue->count--;
		return delProc;
	}
}

void firstInFirstOut(Process *processSet, bool **workLoad, int totalRunningTime){
	Queue readyQue;
	int timer = 0;
	int processIndex = 0;
	Process *runningProcess = NULL;
	initQue(&readyQue);
	//workLoad = initWorkLoad(processCount, totalRunningTime, workLoad);

	for(; timer < totalRunningTime; timer++){
		while(processSet[processIndex].arriveTime == timer){
			enQueueRear(&readyQue, &processSet[processIndex]);
			processIndex++;
		}
		if(runningProcess == NULL){
			runningProcess = deQueue(&readyQue);
		}
		runningProcess -> currentServiceTime++;
		workLoad[runningProcess -> processId][timer] = true;
		if(runningProcess -> serviceTime == runningProcess -> currentServiceTime){
			runningProcess = NULL;
		}
	}
}

void multilevelFeedbackQueue(Process *processSet, bool **workLoad, int totalRunningTime, int timeSlice){
	Queue multiQueue[4];
	int timer = 0;
	int processIndex = 0;
	int queueLevel = 4;
	int processPriority = 0;

	Process *runningProcess = NULL;

	for(int i = 0; i < queueLevel; i++){
		initQue(&multiQueue[i]);
		multiQueue[i].priority = i;
	}
//	workLoad = initWorkLoad(processCount, totalRunningTime, workLoad);

	for(; timer < totalRunningTime;){
		
		while(processSet[processIndex].arriveTime == timer){
			enQueueFront(&multiQueue[0], &processSet[processIndex]);
			processIndex++;
		}
		if(runningProcess == NULL){ //작업중인 큐가 없으면 큐에 새로운 잡을 넣어줌
			for(int i = 0; i < queueLevel; i++){
				runningProcess = deQueue(&multiQueue[i]); // 우선순위가 높은 큐에 있는 잡부터 수행프로세스에 삽입
				processPriority = i;
				if(runningProcess != NULL){ // 러닝프로세스에 들오면 더이상 큐에서 프로세스를 빼지 않음
					break;
				}
			}
		}
		runningProcess -> currentServiceTime++;
		runningProcess -> accumulatedTime++;
		workLoad[runningProcess -> processId][timer] = true;
		timer++; // 타이머 증가
		
		if(runningProcess -> currentServiceTime == runningProcess -> serviceTime){
			runningProcess = NULL; //프로세스 종료
		}else if(runningProcess -> accumulatedTime == timeSlice){ //아직 수행할 시간이 더 남았음
			if(processPriority == (queueLevel - 1)){ 
				processPriority--;
			}

			if(processSet[processIndex].arriveTime == timer + 1 || !isAllEmpty(multiQueue, queueLevel)){
				if(processPriority != (queueLevel - 1)){
					processPriority++;
				}
				enQueueRear(&multiQueue[processPriority], runningProcess);
			}else{
				enQueueRear(&multiQueue[processPriority], runningProcess);
			}
			runningProcess -> accumulatedTime = 0;
			runningProcess = NULL;
		}
	}
}

bool ** createWorkLoad(int totalRunningTime, bool **workLoad){
	workLoad = (bool**) malloc (sizeof(bool*) * PROCESS_COUNT);
    for(int i = 0; i < PROCESS_COUNT; i++){
         workLoad[i] = (bool*) malloc (sizeof(bool) * totalRunningTime);
         for(int j = 0; j < totalRunningTime; j++){
			 workLoad[i][j] = false;
		 }
    }
	return workLoad;
}

void showWorkLoad(int totalRunningTime, bool **workLoad){
	char processName[PROCESS_COUNT] = {'A', 'B', 'C', 'D', 'E'};

	for(int i = 0; i < PROCESS_COUNT; i++){
		printf("%c :", processName[i]);
		for(int j =0; j < totalRunningTime; j++){
			if(workLoad[i][j] == true){
				printf(" ■ ");
			}else{
				printf(" □ ");
			}
		}
		printf("\n");
	}
}

void showMenu(void){
	printf("-------------------------------------\n");
	printf("1. First come first serve\n");
	printf("2. Shortest job first\n");
	printf("3. Round robin\n");
	printf("4. Multilevel feedback queue\n");
	printf("5. Lottery\n");
	printf("6. QUIT\n");
	printf("-------------------------------------\n");
}

void initComponent(Process *processSet, bool **workLoad, int totalServiceTime){
	//Process 설정 초기화
	for(int i = 0; i < PROCESS_COUNT; i++){
		processSet[i].currentServiceTime = 0;
		processSet[i].accumulatedTime = 0;
	}

	//workLoad 초기화
	for(int i = 0; i < PROCESS_COUNT; i++){
		for(int j = 0; j < totalServiceTime; j++){
				workLoad[i][j] = false;
		}
	}
}

