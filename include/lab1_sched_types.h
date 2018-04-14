/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32140033 32163322
*	    Student name : 강민구   이승현
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H


/*
 * You need to Declare functions in  here
 */

#define PROCESS_COUNT 5

typedef struct process{
	int arriveTime; // 프로세스 도착 시간
	int serviceTime; // 프로세스 요구 수행 시간
	int currentServiceTime; // 실제 프로세스 수행 시간
	int accumulatedTime; // MLFQ에 사용될 누적시간
	int processId; // processId
	int ticket;	//for rottery scheduler
	int priority; // MLFQ에서 사용될 우선순위 
}Process;

typedef struct node{
	Process *process; // 프로세스 데이터
	struct node *next; // 다음 노드
}Node;

typedef struct queue{
	Node *front; // 맨 앞(프로세스 꺼낼 위치)
	Node *rear; // 맨 뒤(보관할 위치)
	int count; // 보관 개수
	int timeSlice; //MLFQ에서 큐의 타임슬라이스
	int priority; // MLFQ에서 큐의 우선순위
}Queue;

void initQueue(Queue *queue); // 큐 초기화
bool isEmpty(Queue *queue); // 큐 공백 확인
void enQueueFront(Queue *queue, Process *process); // 큐 front에 프로세스 삽입
void enQueueRear(Queue *queue, Process *process); // 큐 rear에 프러세스 삽입
void enQueueInSJF(Queue *queue, Process *process); //SJF에서 큐 삽입
Process * deQueue(Queue *queue); // 큐에서 프로세스 삭제
void * deQueueInLottery(Queue *queue, Node *pa, Node *ch); //Lottery의 큐에서 프로세스 삭제
 

void firstInFirstOut(Process *process, bool **workLoad, int totalServiceTime); // FIFO 스케쥴러 함수
void multilevelFeedbackQueue(Process *process, bool **workLoad, int totalServiceTime, int timeSlice, bool exp);
void showWorkLoad(int totalRunningTime, bool **workLoad); // workLoad 출력함수
bool ** createWorkLoad(int totalRunningTime, bool **workLoad); 
void showMenu(void);
void initComponent(Process *processSet, bool **workLoad, int totalServiceTime);
void SJF(Process *processSet, int totalServiceTime, bool **workload);
void RR(Process *processSet, int totalServiceTime, bool **workload, int timeSlice);
void Lottery(Process *processSet, int totalServiceTime, bool **workload);

#endif /* LAB1_HEADER_H*/
