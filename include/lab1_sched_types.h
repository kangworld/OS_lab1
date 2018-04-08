/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32140033
*	    Student name : 강민구
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
	int processId;
}Process;

typedef struct node{
	Process *process; // 프로세스 데이터
	struct node *next; // 다음 노드
}Node;

typedef struct queue{
	Node *front; // 맨 앞(프로세스 꺼낼 위치)
	Node *rear; // 맨 뒤(보관할 위치)
	int count; // 보관 개수
	int priority;
}Queue;

void initQueue(Queue *queue); // 큐 초기화
bool isEmpty(Queue *queue); // 큐 공백 확인
void enQueueFront(Queue *queue, Process *process); // 큐 front에 프로세스 삽입
void enQueueRear(Queue *queue, Process *process); // 큐 rear에 프러세스 삽입
Process * deQueue(Queue *queue); // 큐에서 프로세스 삭제

void firstInFirstOut(Process *process, bool **workLoad, int totalServiceTime); // FIFO 스케쥴러 함수
void multilevelFeedbackQueue(Process *process, bool **workLoad, int totalServiceTime, int timeSlice);
void showWorkLoad(int totalRunningTime, bool **workLoad); // workLoad 출력함수
bool ** createWorkLoad(int totalRunningTime, bool **workLoad); 
void showMenu(void);
void initComponent(Process *processSet, bool **workLoad, int totalServiceTime);
#endif /* LAB1_HEADER_H*/
