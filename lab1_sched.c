/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32140033, 32163322
*	    Student name : 강민구,   이승현
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
#include <math.h>
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

/*
 * MLFQ에서 Queue의 timeSlice를 설정하는 함수
 */
void setQueueTimeSlice(Queue *queue, int timeSlice){
	queue -> timeSlice = timeSlice;
}

/*
 * 큐의 공백을 검사하는 함수
 */
bool isEmpty(Queue *queue){
	if(queue->count == 0){
		return true; //공백이면 true
	}else{
		return false; //아니면 false
	}
}

/*
 * MLFQ에서 모든 큐가 공백인지 검사하는 함수
 */
bool isAllEmpty(Queue *multiQueue, int queueLevel){
	for(int i = 0; i < queueLevel; i++){
		if(isEmpty(&multiQueue[i]) == false){
			return false;
		}
	}
	return true;
}

/*
 * 큐의 rear에 노드를 삽입하는 함수
 */
void enQueueRear(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
		queue->rear = newNode;
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
			if(pa->process->serviceTime <= process->serviceTime && ch->process->serviceTime <= process->serviceTime){
				newNode->next = ch;
				pa->next = newNode;
				printf("1\n");
				break;
			}else{
				printf("2\n");
				pa = pa->next;
				ch = ch->next;
			}
		}
			if(ch == NULL){ //맨 앞, 맨 뒤에 삽입하는 경우
				if(queue->rear->process->serviceTime < process->serviceTime){//맨 뒤 삽입
		                	queue->rear->next = newNode;
					queue->rear = newNode;
					printf("3\n");
                  		}else{ //노드가 하나일 경우, 맨 앞 삽입
                          		newNode->next = queue->front;
					queue->front = newNode;
					printf("4\n");
				}
			}
	}
	queue->count++;
}

/*
 * 큐의 head에 노드를 삽입하는 함수
 */
void enQueueFront(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
		queue->rear = newNode;
	}else{
		newNode->next = queue->front;
		queue->front = newNode;
	}
	queue->count++;
}

/*
 * 큐의 head 노드를 삭제하는 함수
 */
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

void * deQueueInLottery(Queue *queue, Node *pa, Node *ch){
	if(!pa){ //front삭제
		queue->front = ch->next;
		free(ch);
	}else if(!(ch->next)){ //rear삭제
		queue->rear = pa;
		pa->next = NULL;
		free(ch);
	}else{ //중간 노드 삭제
		pa->next = ch->next;
		free(ch);
	}
	queue->count--;

}

/*
 * FIFO
 * processSet : 스케쥴링의 대상이 되는 프로세스 집합
 * workLoad : 프로세스들의 수행결과를 저장할 워크로드
 * totalRunningTime : 대기중인 프로세스들의 총 수행시간
 */
void firstInFirstOut(Process *processSet, bool **workLoad, int totalRunningTime){
	Queue readyQue;
	int timer = 0;
	int processIndex = 0;
	Process *runningProcess = NULL;
	initQue(&readyQue);

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

/*
 * MLFQ
 * processSet : 스케쥴링의 대상이 되는 프로세스 집합
 * workLoad : 프로세스들의 수행결과를 저장할 워크로드
 * totalRunningTime : 대기중인 프로세스들의 총 수행시간
 * timeSlice : 작업 할당량
 * exp : 큐의 우선순위가 낮아질수록  timeSlice를 제곱함 
 */
void multilevelFeedbackQueue(Process *processSet, bool **workLoad, int totalRunningTime, int timeSlice, bool exp){
	Queue multiQueue[4];
	int timer = 0;
	int expedNumber = timeSlice;
	int processIndex = 0;
	int queueLevel = 4;
	int processPriority = 0;
	Process *runningProcess = NULL;

	for(int i = 0; i < queueLevel; i++){
		initQue(&multiQueue[i]);
		multiQueue[i].priority = i;
		if(exp == true){
			timeSlice = pow(expedNumber, i);
		}
		multiQueue[i].timeSlice = timeSlice;
	}

	for(; timer < totalRunningTime;){
		
		while(processSet[processIndex].arriveTime == timer){
			enQueueFront(&multiQueue[0], &processSet[processIndex]);
			processIndex++;
		}
		if(runningProcess == NULL){ //작업중인 큐가 없으면 큐에 새로운 잡을 넣어줌
			for(int i = 0; i < queueLevel; i++){
				runningProcess = deQueue(&multiQueue[i]); // 우선순위가 높은 큐에 있는 잡부터 수행프로세스에 삽입
			
				processPriority  = i;
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
		}else if(runningProcess -> accumulatedTime == multiQueue[processPriority].timeSlice){ //아직 수행할 시간이 더 남았음
			if(isAllEmpty(multiQueue, queueLevel) == false){
				if( processPriority   != (queueLevel - 1)){
					printf("gi\n");		
					processPriority ++;
					printf("id : %d rpit : %d\n", runningProcess->processId, processPriority );
				}
			}
			enQueueRear(&multiQueue[processPriority], runningProcess);
			
			runningProcess -> accumulatedTime = 0;
			runningProcess = NULL;
		}
	}
}

void SJF(Process *processSet, int totalServiceTime, bool **workload){
	Queue ReadyQ;
	int index=0; //프로세스 진입 확인
	Process *RunProc;


	initQue(&ReadyQ);
	
	for(int time = 0;time<totalServiceTime;time++){
		for(;index<PROCESS_COUNT;index++){ //도착시 readyQ에 삽입
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

void RR(Process *processSet, int totalServiceTime, bool **workload, int timeSlice){
	Queue ReadyQ;
	int index=0;
	int run=0; //timeslice 체크를 위한 변수
	Process *RunProc=NULL;
	
	initQue(&ReadyQ);

	for(int time=0;time<totalServiceTime;time++){
		for(;index<PROCESS_COUNT;index++){ //arriveTime에 맞추어 큐에 삽입
			if(processSet[index].arriveTime == time){
				enQueueRear(&ReadyQ, &processSet[index]);
			}else break;
		}
		if(run == timeSlice){
			run = 0;
			enQueueRear(&ReadyQ, RunProc);
		}

		printf("time : %d\n", time);
		if(run == 0){
			RunProc = deQueue(&ReadyQ);
		}
		printf("1\n");
		(RunProc->currentServiceTime)++;
		workload[RunProc->processId][time] = true;
		run++;
		if(RunProc->currentServiceTime == RunProc->serviceTime){ //수행이 덜 
			run = 0;
			printf("2\n");
		}

	}
}

void Lottery(Process *processSet, int totalServiceTime, bool **workload){
	Queue ReadyQ;
	int index = 0;
	int totalTicket = 0;
	int lotteryNum;
	int search;
	Process *forSearch;
	Node *pa;
	Node *ch;

	srand(time(NULL)); //seed값 사용

	initQue(&ReadyQ);

	for(int time=0;time<totalServiceTime;time++){
		for(;index<PROCESS_COUNT;index++){ //arrive time에 맞추어 큐에 삽입
			if(processSet[index].arriveTime == time){
				enQueueRear(&ReadyQ, &processSet[index]);
				totalTicket += processSet[index].ticket;
			}else break;
		}

		if(!isEmpty(&ReadyQ)){
			lotteryNum = rand()%totalTicket;
			forSearch = ReadyQ.front->process;
			search = 0;
			pa = NULL;
			ch = ReadyQ.front;
			for(int i = 0; i<ReadyQ.count;i++){ //어느 노드인지 탐색
				search += forSearch->ticket;
				if(search > lotteryNum){ //찾았을 경우
					forSearch->currentServiceTime++;
					workload[forSearch->processId][time]=true;

					if(forSearch->currentServiceTime == forSearch->serviceTime){//수행완료시
						totalTicket -= forSearch->ticket;
						deQueueInLottery(&ReadyQ, pa, ch);
					}
					break;
				}
				pa = ch;
				ch = ch->next;
				forSearch = ch->process;
			}
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
