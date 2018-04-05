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

void enQueue(Queue *queue, Process *process){
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
