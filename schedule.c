#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_THREADS 100

int NUM_THREADS = 10;
int NUM_TICKETS = 0;

int cant[MAX_THREADS];
int tickets[MAX_THREADS];
int ticket_sum[MAX_THREADS];


int select_ticket(int);
void calculate_ticket_sum();
void remove_tickets(int);
int choose_winner();

int main(int argc, char** argv){
    int test = 1000000;
    for(int i=0;i<NUM_THREADS;i++){
        tickets[i] = 1;
        if(i == 3)
            tickets[i] += 1;
    }
    calculate_ticket_sum();
    while(test--){
        if(NUM_TICKETS == 0)
            break;
        int to_run = choose_winner();
        cant[to_run]++;
        if(cant[to_run] == 80000){
            remove_tickets(to_run);
        }
            
    }

    for(int i=0;i<10;i++){
        printf("Value %d times: %d\n",i,cant[i]);
    }
    return 0;
}


int select_ticket(int max_val){
    return (rand()/ (RAND_MAX / max_val + 1));
}

void calculate_ticket_sum(){
    ticket_sum[0] = tickets[0];
    for(int i =1; i < NUM_THREADS; i++){
        ticket_sum[i] = ticket_sum[i-1]+tickets[i];
    }
    NUM_TICKETS = ticket_sum[NUM_THREADS-1];
}

void remove_tickets(int pid){
    tickets[pid] = 0;
    calculate_ticket_sum();
}

int choose_winner(){
    int winner = select_ticket(NUM_TICKETS);
    int increment = sqrt(NUM_THREADS);
    for(int i=0;i<NUM_THREADS;i++){
        if(ticket_sum[i] > winner)
            return i;
    }
    return -1;
}