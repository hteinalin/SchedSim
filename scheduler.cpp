// ---------------------------------------------------------
/*
	File: scheduler.cpp
	Package: SchedulerSim
 
	Author: John Pecarina
	Date: 17 July 2015
 
	Description: A simple dispatcher function for a computer.
	License/Copyright: None
 */
//---------------------------------------------------------
/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "TCB.h"
#include "TCB_Queue.h"
//Use the standard namespace
using namespace std;


const int SchedulerType = 0;
// 0 for FCFS/RoundRobin
// 1 for Shortest Job First


/*--------------------------------------------------------------------------*/
/* CLASS METHODS */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler(){
    //Intialize the new, ready, wait and terminated queues
    nq = new TCB_Queue(MAX_QUEUE_SIZE); //new_queue;
    rq = new TCB_Queue(MAX_QUEUE_SIZE); //ready_queue;
    wq = new TCB_Queue(MAX_QUEUE_SIZE); //wait_queue;
    tq = new TCB_Queue(MAX_QUEUE_SIZE); //terminated_queue;
    last_run = 0;
    times_called = 0;
    
    //build an idle TCB node
    char *nop = "N";
    idle = tq->createTCBnodePID(0, nop);
}

void Scheduler::load_new_job(uint8_t pid, char* program){
    //Fill the new queue with jobs
    TCBnode* node;
    node=nq->createTCBnodePID(pid, program);
    //node->tcb.instructionSize = *program;
    nq->enqueue(node);
    //nq->display();
}

void Scheduler::load_last_job(uint8_t pid, char* program){
    //Set the last TCB (which will exit when needed)
    last=nq->createTCBnodePID(pid, program);
}


// Check the wait queue

void Scheduler::waitQueueCode(){
    //Moving Wait Queue nodes to temporary array
    TCBnode** arrayForSortingWaitQueue;
    arrayForSortingWaitQueue = new TCBnode *[wq->get_size()];
    int waitQueueSize = wq->get_size();
    int queueCounter = 0;
    while(wq->get_size()){
        arrayForSortingWaitQueue[queueCounter] = wq->dequeue();
        queueCounter++;
    }
    
    //Sorting time in ascending order using selection sort if Scheduler Type is SJF
    
    if(SchedulerType == 1){
        TCBnode* tempSortNode;
        for(int i = 0; i < waitQueueSize; i++){
            int currentPosition = i;
            for(int j = i + 1; j < waitQueueSize; j++){
                if(arrayForSortingWaitQueue[j]->tcb.instruction_counter < arrayForSortingWaitQueue[currentPosition]->tcb.instruction_counter)
                    currentPosition = j;
            }
            tempSortNode = arrayForSortingWaitQueue[i];
            arrayForSortingWaitQueue[i] = arrayForSortingWaitQueue[currentPosition];
            arrayForSortingWaitQueue[currentPosition] = tempSortNode;
        }
    }
    
    //Moving array items to Ready Queue
    for(int i = 0; i < waitQueueSize; i++)
        rq->enqueue(arrayForSortingWaitQueue[i]);
}



void Scheduler::run_scheduler(){
    printf("In SCHEDULER::RunScheduler\n");
    times_called++;
    
    // Round Robin/FCFS
    if(SchedulerType == 0){
        while(nq->get_size()){
            TCBnode* node;
            node = nq->dequeue();
            rq->enqueue(node);
            printf("PID: %d\n", node->tcb.pid);
        }
        waitQueueCode();
    }
    // SJF
    else if(SchedulerType == 1){
        //Moving New Queue nodes to temporary array
        TCBnode** arrayForSortingNewQueue;
        arrayForSortingNewQueue = new TCBnode *[nq->get_size()];
        int newQueueSize = nq->get_size();
        int queueCounter = 0;
        while(nq->get_size()){
            arrayForSortingNewQueue[queueCounter] = nq->dequeue();
            queueCounter++;
        }
        
        //Sorting time in ascending order using selection sort
        TCBnode* tempSortNode;
        for(int i = 0; i < newQueueSize; i++){
            int currentPosition = i;
            for(int j = i + 1; j < newQueueSize; j++){
                if(arrayForSortingNewQueue[j]->tcb.instruction_counter < arrayForSortingNewQueue[currentPosition]->tcb.instruction_counter)
                    currentPosition = j;
            }
            tempSortNode = arrayForSortingNewQueue[i];
            arrayForSortingNewQueue[i] = arrayForSortingNewQueue[currentPosition];
            arrayForSortingNewQueue[currentPosition] = tempSortNode;
        }
        
        //Moving array items to Ready Queue
        for(int i = 0; i < newQueueSize; i++)
            rq->enqueue(arrayForSortingNewQueue[i]);
        waitQueueCode();
    }
}

bool Scheduler::has_remaining_jobs(){
	if (nq->get_size())
		return true;
	if (rq->get_size())
		return true;
	if (wq->get_size())
		return true;

	//if we reach this point, all of the queues are empty
	return false;
}


//One way you could do this...
//resume_threads();	//moves threads from wait if signals rec'd
//add_new_threads();	//moves threads from new to ready
//sort_ready_queue();	//sort the ready queue from the dispatcher
/* sort_ready_queue() can be implemented in several ways depending on the Algorithm requested */
;

TCBnode** Scheduler::get_last_TCBnode(){return &last;}
TCBnode** Scheduler::get_idle_TCBnode(){return &idle;}
void Scheduler::set_idle_TCBnode(TCBnode ** _node){idle = *_node;}
void Scheduler::set_last_run(long l){last_run = l;}
long Scheduler::get_last_run(){	return last_run;}
int Scheduler::get_times_called(){ return times_called;}



