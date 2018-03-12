#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifndef EXPROPIATIVE_SCHEDULER_H
#define EXPROPIATIVE_SCHEDULER_H

struct ExpropiativeScheduler{
	int *tickets;
	int *ticket_sum;
	int size;
	int num_tickets;
};
/*
void calculate_ticket_sum(struct ExpropiativeScheduler *scheduler){
    scheduler->ticket_sum[0] = scheduler->tickets[0];
    for(int i =1; i < scheduler->size; i++){
        scheduler->ticket_sum[i] = scheduler->ticket_sum[i-1]+scheduler->tickets[i];
    }
    scheduler->num_tickets = scheduler->ticket_sum[scheduler->size-1];
}*/

void initExpropiativeScheduler(struct ExpropiativeScheduler *scheduler, struct Property *property){
	printf("initExpropiativeScheduler\n");
	printf("size: %d\n",property->size);
	scheduler->tickets = property->tickets;
	scheduler->size = property->size;
	scheduler->num_tickets = 0;
	scheduler->ticket_sum = malloc(property->size*sizeof(int));
	memset(scheduler->ticket_sum, 0, property->size*sizeof(int));
	//calculate_ticket_sum(scheduler);
}
/*
int select_ticket(int max_val){
    return (rand()/ (RAND_MAX / max_val + 1));
}

void remove_tickets(struct ExpropiativeScheduler *scheduler,int pid){
    scheduler->tickets[pid] = 0;
    calculate_ticket_sum(scheduler);
}

int choose_winner(struct ExpropiativeScheduler *scheduler){
    int winner = select_ticket(scheduler->num_tickets);
    int increment = sqrt(scheduler->size);
    for(int i=0;i<scheduler->size;i++){
        if(scheduler->ticket_sum[i] > winner)
            return i;
    }
    return -1;
}*/


#endif