/* 
    File: kernel.cpp
		Package: SchedulerSim

    Author: John Pecarina
    Date  : 07/14/15

		System: Linux Ubuntu 14.04
		Compiler: g++
		External Files Required: N/A

    This file has the main entry point to the operating system.

    MAIN FILE FOR CSCE 489 SCHEDULER LAB
		License/Copyright: None
*/
//---------------------------------------------------------

/*-------------------------------------------------------------------------*/
/* INCLUDES */
/*-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include "SchedSim.h"
#include "CPU.h"	     /* MACHINE */
#include "scheduler.h"	    /* PROCESS MANAGEMENT */
#include "dispatcher.h"
/*--------------------------------------------------------------------------*/
/* Global Variables */
/*--------------------------------------------------------------------------*/

/* -- CPU pointer */
CPU* cpu;

/* -- The system scheduler */
Scheduler* sched;

/* -- The dispatcher */
Dispatcher* ds;

/*-----------------------------------------------------------------
   RANDOM THREAD SIMULATOR  -----------------------------------------------------------------*/

void init(){

	srand(time(NULL)); //Seed
	int r_burst_length, w_bursts;// Random Variables
	int i, j, k, str_idx;
	//thread 0 was idle, so start PID at 1 and goto INIT_PROGRAMS+1
	for(i = 1; i<INIT_PROGRAMS+1; i++)
	{
		w_bursts = 1 + rand() % MAX_CPU_BURSTS;
		char *thread_str = (char *) malloc(w_bursts * MAX_CPU_BURST_LENGTH);
		if (thread_str) {
			str_idx = 0;
			for(j = 0; j<w_bursts; j++)
			{	
				r_burst_length = 1 + rand() % MAX_CPU_BURST_LENGTH;
				for(k = 0; k<r_burst_length; k++)
				{	      			
					thread_str[str_idx] = 'E';
					str_idx++;				
				}
				if (j == w_bursts-1)
					thread_str[str_idx] = 'T';
				else
					thread_str[str_idx] = 'W';
				str_idx++;
			}
			thread_str[str_idx] = '\0';			
			//printf("%s\n", thread_str);
			sched->load_new_job(i, thread_str);
		}
	}

	//Add the shutdown process last
	char* end = "X";	
	sched->load_last_job(INIT_PROGRAMS+1, end);
	//this is for the scheduler to exit when the scheduler chooses
	
	//runs the scheduler the first time
	ds->choose_next_thread();
	//load the first process on the CPU
	ds->load_state_of_CPU();
}

void print_stats(){
	long numProcesses = sched->tq->get_size();
        long actualWaitTime = 0;
        long totalWaitTime = 0;
	long totalTurnaroundTime = 0;
	long totatlInstructionCount = 0;           
           while (sched->tq->get_size()) {
                TCBnode* node;
                node = sched->tq->dequeue();
                long actualTurnaroundTime = node->tcb.turnaround_time - node->tcb.wait_time;
		long instructionCount = node->tcb.instruction_counter;
                printf("Process PID: %d\n", node->tcb.pid);
                printf("     Process instruction count: %d\n", instructionCount);
                printf("     NQ create time: %lu\n", node->tcb.create_time);
                printf("     RQ arrival time: %lu\n", node->tcb.arrive_time);
                printf("     First time at processor: %lu\n", node->tcb.wait_time);
                printf("     Actual Wait time: %lu\n", actualWaitTime);
                printf("     Turnaround time: %lu\n", node->tcb.turnaround_time);
                printf("     Actual Turnaround time: %lu\n", actualTurnaroundTime);
                actualWaitTime += actualTurnaroundTime;
                totalWaitTime += actualWaitTime;
	        totalTurnaroundTime += actualTurnaroundTime;
		totatlInstructionCount += instructionCount;
            }
            printf("\n - Average Wait Time: %lu - \n", totalWaitTime/numProcesses);
	    printf(" - Average Turnaround Time:  %lu -\n", totalTurnaroundTime / numProcesses);	
	    printf(" - Average Instruction Count:  %lu -\n", totatlInstructionCount / numProcesses);
		

	    int sched_cost = sched->get_times_called() * SCHED_COST;
	    int disp_cost = ds->get_times_called() * CONTEXT_SWITCH_COST;
	    printf("Scheduler ran %d times for %d cost \n", sched->get_times_called(), sched_cost);
	    printf("Dispatcher ran %d times for %d cost \n", ds->get_times_called(), disp_cost);

	    double throughput =  ((double) (*cpu->get_time() - sched_cost - disp_cost)) / (double) *cpu->get_time();
	    printf("The algorithm throughput was %f%% efficient.\n", throughput*100);
        
}

/*--------------------------------------------------------------------------*/
/* MAIN ENTRY INTO THE OS */
/*--------------------------------------------------------------------------*/

int main(){

	//Provision a CPU
	cpu = new CPU;

	//Provision a Scheduler for the CPU
	sched = new Scheduler;

	//Provision the dispatcher
	ds = new Dispatcher(&cpu, &sched);

	//A welcome banner	
	printf("Dispatcher on...Calling init()");

	//Call init to load user programs into NEW queue
	init();

	//All is setup, Entering the main loop
	printf("Entering main dispatch loop...\n");
	//A constant loop until the shutdown instruction: 'X'
	while (ds->run_thread() && sched->has_remaining_jobs()){ // Runs a thread that is currently on the CPU
		// save the state of the process
		ds->save_state_of_CPU();
		//Gets the next thread from the ready queue
		ds->choose_next_thread();	
		//Loads state from new TCB onto the CPU
		ds->load_state_of_CPU(); 

	}
	//a hanging prompt that waits for the user to exit
	printf("System shutting down\n");
	printf("New Queue size = %d\n",sched->nq->get_size());
	printf("Ready Queue size = %d\n",sched->rq->get_size());
	printf("Wait Queue size = %d\n",sched->wq->get_size());
	printf("Terminated Queue size = %d\n",sched->tq->get_size());
	//one might output other statistics here...
	print_stats();
	printf("Press enter to exit:");
	scanf("Enter");
	

	return 1;
}
  

