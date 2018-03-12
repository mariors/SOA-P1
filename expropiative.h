#include "loader.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "expropiative_scheduler.h"

#ifndef EXPROPIATIVE_H
#define EXPROPIATIVE_H

static struct ExpropiativoScheduler scheduler;
struct Property *global_property;


void do_expropiative(struct ExpropiativoScheduler *scheduler){
	// sacar thread
	int newThread = choose_winner(scheduler);
	//send to Run
	printf("do_expropiative thread: %d\n",newThread);
}


void timer_handler (int signum){
	static int count = 0;
	static int lock = 0;
	if(lock==0){
		lock++;
		initExpropiativoScheduler(&scheduler,global_property);
	}else{
		do_expropiative(&scheduler);
		//printf ("timer expired %d times\n", ++count);
	}
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

void run_expropiative(struct Property *property){
	global_property = property;
    create_signal_timer(property->quantum);
    keep_working();
}

#endif