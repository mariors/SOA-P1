#include "loader.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "expropiative_scheduler.h"
#include "schedule.c"

#ifndef EXPROPIATIVE_H
#define EXPROPIATIVE_H


// Registar buffers de los threads
static struct ExpropiativeScheduler scheduler;
struct Property *global_property;


//quantum
void do_expropiative(struct ExpropiativeScheduler *scheduler){
    int b = checkIfThreadRunning();
    printf("do_expropiative: %d\n",b);
    if(b){
        // longjmp(sched,TIMEOUT);
        interrupted=1;    
    }
	// sacar thread
    // check if current thread DONE y remove_ticket
    // change state to SUSPENDED
	//int newThread = choose_winner(scheduler); // escoje el nuevo Thread
	//send to Run
	//printf("do_expropiative thread: %d\n",newThread);
}



void timer_handler (int signum){
	do_expropiative(&scheduler);
}

void create_signal_timer(int milliseconds){
    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa, NULL);

    /* Configure the timer to expire after 250 msec... */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = milliseconds;
    /* ... and every 250 msec after that. */
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = milliseconds;
    /* Start a virtual timer. It counts down whenever this process is
    executing. */
    setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

void keep_working(){
    while (1);
}

/*
void run_expropriative(struct Property *property){
	global_property = property;
    //initExpropiativeScheduler(&scheduler,global_property);
    create_signal_timer(property->quantum);
    keep_working();
}*/

#endif