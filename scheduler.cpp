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
#include <string.h>
#include <math.h>
#include "scheduler.h"
#include "TCB.h"
#include "TCB_Queue.h"
//Use the standard namespace
using namespace std;

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

		//build an idle TCB node
		char *nop = "N";	
		idle = tq->createTCBnodePID(0, nop);
	}

	void Scheduler::load_new_job(uint8_t pid, char* program){
		//Fill the new queue with jobs
		TCBnode* node;
		node=nq->createTCBnodePID(pid, program);
		nq->enqueue(node);
		//nq->display();
	}

	void Scheduler::load_last_job(uint8_t pid, char* program){
		//Set the last TCB (which will exit when needed)
		last=nq->createTCBnodePID(pid, program);
        //nq->enqueue(last);
	}

    void Scheduler::resume_threads(){
        //Will move threads from wait queue to ready queue
        if(wq->get_size() == 0)
        {
            //Empty wait queue
        }
        else
        {
            rq->enqueue(wq->dequeue());
        }
    }

    void Scheduler::add_new_threads(){
        //first check if newqueue is empty, if so
        if(nq->get_size() == 0)
        {
            return;
        }
        
        while(nq->get_size()!=0)
        {
            rq->enqueue(nq->dequeue());
        }
    }

    void Scheduler::sort_ready_queue(){
        TCB_Queue* tempq = new TCB_Queue(MAX_QUEUE_SIZE); //temp_queue for sorting;
        TCBnode* node1;
        TCBnode* node2;
        char* program;
        int min = MAX_CPU_BURST_LENGTH+2;     //Min will always be less then MAX_CPU_BURST_LENGTH+1
        int length;
        
        //First Copy all of the ready queue into tempq
        while(rq->get_size()!=0)
        {
            tempq->enqueue(rq->dequeue());
        }

        //Set node1 to head of tempq
        node1 = tempq->getHead();
        program = node1->tcb.program_ctr;
        length = strlen(program);
        
        //Loop through tempq n times to order them into rq, do so until tempq is empty
        while(tempq->get_size()!= 0)//rq->get_size() != tempq->get_size())
        {
            //Now go through tempq to find min, put in ready que, repeat until tempq is empty
            while(node1 != NULL)//node1 != tempq->getTail())
            {
                if(length < min)
                {
                    min = length;
                    node2 = node1;
                }
                
                //Advance node and variables
                node1 = node1->next;
                if(node1 == NULL);  //Don't advance variables becasue they will not exist if node1 == NULL
                else
                {
                    program = node1->tcb.program_ctr;
                    length = strlen(program);
                }
            }
        
            //Now that we have the minimum node, put in ready que, then continue until tempq empty
            rq->enqueue(tempq->pullNode(node2));
            
            if(tempq->get_size() == 0) ;//Reset values, unless the queue is empty
            else
            {
                min = MAX_CPU_BURST_LENGTH+2;     //Reset min value
                node1 = tempq->getHead();       //Reset node1 to start at head again
                program = node1->tcb.program_ctr;
                length = strlen(program);
            }
        }
    }

	void Scheduler::run_scheduler(){
		printf("In SCHEDULER::RunScheduler\n");
		//One way you could do this...
		//resume_threads();	//moves threads from wait if signals rec'd
		add_new_threads();	//moves threads from new to ready
		sort_ready_queue();	//sort the ready queue from the dispatcher
		/* sort_ready_queue() can be implemented in several ways depending on the Algorithm requested */
		;
	}

	TCBnode** Scheduler::get_last_TCBnode(){return &last;}
	TCBnode** Scheduler::get_idle_TCBnode(){return &idle;}
	void Scheduler::set_idle_TCBnode(TCBnode ** _node){idle = *_node;}
	void Scheduler::set_last_run(long l){last_run = l;}
	long Scheduler::get_last_run(){	return last_run;}



