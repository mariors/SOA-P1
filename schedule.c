#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include "loader.h"
#include "arctan_func.h"

#define MAX_THREADS 100
#define NOT_STARTED 0
#define RUNNING 1
#define SUSPENDED 2
#define DONE 3
#define TIMEOUT 4

int num_tickets;

int num_threads;
int quantum;
int expropriative_mode;
int* cant;
int* tickets;
int* ticket_sum;
int* thread_status;
int* workload;

//Series calculation.
long double* temp_acc;
long double* thread_acc;
// jmp_buf** bufs;
jmp_buf bufs[5];
jmp_buf sched;

int select_ticket(int);
void calculate_ticket_sum();
void remove_tickets(int);
int choose_winner();
long double run_work_unit(int,int);
void run_thread(volatile int pid);

void initialize_global(struct Property* property) {
    num_threads = property->size;
    cant = malloc(sizeof(int)*num_threads);
    tickets = property->tickets;
    quantum = property->quantum;
    expropriative_mode = property->mode;
    ticket_sum = malloc(sizeof(int)*num_threads);
    thread_status = malloc(sizeof(int)*num_threads);
    temp_acc = malloc(sizeof(long double)*num_threads);
    thread_acc = malloc(sizeof(long double)*num_threads);
    // bufs = malloc(sizeof(jmp_buf)*num_threads);
    workload = property->workload;
    for (int i = 0; i < num_threads; ++i) {
        jmp_buf aux;
        cant[i]=0;
        ticket_sum[i]=0;
        thread_status[i]=NOT_STARTED;
        temp_acc[i] = 0.0;
        thread_acc[i] = 0.0;
        // bufs[i] = malloc(sizeof(jmp_buf));
        // bufs[i] = &aux;
    }
    calculate_ticket_sum();
}

int checkIfThreadRunning(){
    int b = 0;
    for(int i = 0; i < num_threads && !b; i++){
        if(thread_status[i]!=NOT_STARTED)
            b = 1;
    }
    return b;
}


int select_ticket(int max_val){
    return (rand()/ (RAND_MAX / max_val + 1));
}

void calculate_ticket_sum(){
    ticket_sum[0] = tickets[0];
    for(int i =1; i < num_threads; i++){
        ticket_sum[i] = ticket_sum[i-1]+tickets[i];
    }
    num_tickets = ticket_sum[num_threads-1];
}

void remove_tickets(int pid){
    tickets[pid] = 0;
    calculate_ticket_sum();
}

int choose_winner(){
    int winner = select_ticket(num_tickets);
    for(int i=0;i<num_threads;i++){
        if(ticket_sum[i] > winner)
            return i;
    }
    return -1;
}

long double calculate_series_element(int iter) {return 1.0;}

void run_thread(volatile int pid) {
    printf("Running %d quantum %d\n",pid,quantum);
    int r;
    int total_iterations = workload[pid] * MIN_WORKLOAD;
    int yield_rate = (quantum*total_iterations/100.0);
    // printf("Yield rate: %d on quantum %d\n",yield_rate,quantum);
    // printf("Running %d\n",pid);
    for (int workit = 1; workit < total_iterations+1; ++workit) {
        // printf("Starting iteration %d\n",pid);
        // printf("inside loop %d\n",pid);
        thread_acc[pid] += arctan_aproximation(1,workit);
        //printf("PID %d it %d: %Lf\n",pid,workit,thread_acc[pid]);
        // printf("Calculated one step %d\n",pid);
        // temp_acc[pid] = 0.0;
        if(workit >0 && workit % yield_rate == 0 && workit%MIN_WORKLOAD==0 && !expropriative_mode){
            // printf("enough steps to yield\n");
            r = setjmp(bufs[pid]);
            if(r==0) longjmp(sched,SUSPENDED); 
            else continue;
        }
    }
    // printf("Thread %d done \n",pid);
    // printf("Final value of thread %d: %Lf\n",pid,thread_acc[pid]);
    // r = setjmp(bufs[pid]);
    longjmp(sched,DONE);
}

int threads_done() {
    for (int i = 0; i < num_threads; ++i){
        if(thread_status[i]!=DONE) return 0;
    }
    return 1;
}

void call_thread(int pid) {
    printf("Calling to run: %d\n",pid);
    if(thread_status[pid] == NOT_STARTED){
        thread_status[pid] = RUNNING;
        printf("Thread %d not started, calling func!\n",pid);

        //ASsign workload to thread
        run_thread(pid);
    } else {
        if(thread_status[pid] != DONE) {
            printf("Resuming: %d\n",pid);
            thread_status[pid] = RUNNING;
            printf("Calling lonjmp resume\n");
            longjmp(bufs[pid],RUNNING);
        } else {
            printf("Thread %d already done \n",pid);
        }
    }
}


void run_non_expropriative(){
    int r;
    while(!threads_done()){
        int to_run = choose_winner();
        r = setjmp(sched);
        if(r==0) call_thread(to_run);
        else {//Returning from thread.
            thread_status[to_run] = r;
            if(thread_status[to_run] == DONE){
               remove_tickets(to_run); 
            }
        }
    }
    for (int i = 0; i < num_threads; ++i){
        printf("PID: %d Result: %Lf\n",i,thread_acc[i]);
    }

}


void run_expropriative(){
    int r;
    while(!threads_done()){
        int to_run = choose_winner();
        r = setjmp(sched);
        printf("Came back from timeout");
        if(r==0) call_thread(to_run);
        else {//Returning from thread.
            if(r==TIMEOUT){ //triggered by timeout func.
                thread_status[to_run] = SUSPENDED;    
            } else { //triggered by thread notification
                thread_status[to_run] = r;
                if(thread_status[to_run] == DONE){
                   remove_tickets(to_run); 
                }
            }
        }
    }
}

/*
void do_timeout(){
     longjmp(sched,TIMEOUT);
}*/
/*
int main(int argc, char** argv){
    //Initialize running environment.
    struct Property property;
    initProperty(&property);
    initialize_global(&property);

    run_non_expropriative();
    return 0;
}*/



long double calculate_arctan(struct Property* property) {
    while(!threads_done(num_threads)){
        //Schedule choose next player.
    }
    long double res = 0.0;
    for (int i = 0; i < num_threads; ++i){
        res+=thread_acc[i];
    }
    return res;
}