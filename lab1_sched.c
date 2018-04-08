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
		return true; //공백이면 true
	}else{
		return false; //아니면 false
	}
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

void enQueueInSJF(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;
	Node *pa = queue->front;//노드찾는 용도
	Node *ch = queue->front;
	

	if(ch == NULL){ //노드가 비었을 경우
		queue->front=newNode;
		queue->rear=newNode;
	}else{
		ch = pa->next;
		for(;ch != NULL;){ //노드가 하나 이상  있을시, 중간에 삽입할 경우
			if(pa->process->serviceTime < process->serviceTime && ch->process->serviceTime < process->serviceTime){
				newNode->next = ch;
				pa->next = newNode;
				break;
			}else{
				pa = pa->next;
				ch = ch->next;
			}
		}
			if(ch == NULL){ //맨 앞, 맨 뒤에 삽입하는 경우
				if(queue->rear->process->serviceTime < process->serviceTime){//맨 뒤 삽입
		                	queue->rear->next = newNode;
					queue->rear = newNode;
                  		}else{ //노드가 하나일 경우, 맨 앞 삽입
                          		newNode->next = queue->front;
					queue->front = newNode;
				}
			}
	}
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


void SJF(Process *processSet, int processCount, int totalServiceTime, bool **workload){
	Queue ReadyQ;
	int index=0; //프로세스 진입 확인
	Process *RunProc;


	initQue(&ReadyQ);
	
	for(int time = 0;time<=totalServiceTime;time++){
		for(;index<processCount;index++){ //도착시 readyQ에 삽입
			if(processSet[index].arriveTime <= time){
				enQueueInSJF(&ReadyQ, &processSet[index]);
			}else break;
		}
		if(!isEmpty(&ReadyQ)){ //큐에 프로세스가 있을시 수행
			RunProc = deQueue(&ReadyQ);
			for(int i = 0; i<RunProc->serviceTime;i++){ //show를 위한 체크
				workload[RunProc->processId][time+i]=true;
			}
			time += RunProc->serviceTime-1;

		}
	}
		
}

void RR(Process *processSet){

}

void Lottery(Process *processSet){

}



bool ** createWorkLoad(int processCount, int totalRunningTime, bool **workLoad){
    workLoad = (bool**) malloc (sizeof(bool*) * processCount);
    for(int i = 0; i < processCount; i++){
         workLoad[i] = (bool*) malloc (sizeof(bool) * totalRunningTime);
         for(int j = 0; j < totalRunningTime; j++){
             workLoad[i][j] = false;
         }
    }
    return workLoad;
}

void showWorkLoad(int processCount, int totalRunningTime, bool **workLoad){
    char processName[5] = {'A', 'B', 'C', 'D', 'E'};

    for(int i = 0; i < processCount; i++){
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
