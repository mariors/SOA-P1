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

int num_tickets;

int num_threads;
int* cant;
int* tickets;
int* ticket_sum;
int* thread_status;
int* workload;

//Series calculation.
long double* temp_acc;
long double* thread_acc;
jmp_buf** bufs;
jmp_buf sched;

int select_ticket(int);
void calculate_ticket_sum();
void remove_tickets(int);
int choose_winner();
long double run_work_unit(int,int);
void run_thread(int pid);

void initialize_global(struct Property* property) {
    num_threads = property->size;
    cant = malloc(sizeof(int)*num_threads);
    tickets = property->tickets;
    ticket_sum = malloc(sizeof(int)*num_threads);
    thread_status = malloc(sizeof(int)*num_threads);
    temp_acc = malloc(sizeof(long double)*num_threads);
    thread_acc = malloc(sizeof(long double)*num_threads);
    bufs = malloc(sizeof(jmp_buf));
    workload = property->workload;
    for (int i = 0; i < num_threads; ++i) {
        jmp_buf aux;
        cant[i]=0;
        ticket_sum[i]=0;
        thread_status[i]=NOT_STARTED;
        temp_acc[i] = 0.0;
        thread_acc[i] = 0.0;
        bufs[i] = &aux;
    }
    calculate_ticket_sum();
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

//Calculate the partial result for one work unit.
long double run_work_unit(int pid,int start) {
    int r;
    for (int i = start; i < start + MIN_WORKLOAD; ++i) {
        r = setjmp(*bufs[pid]);//Might be preempted at any moment.
        temp_acc[pid] += calculate_series_element(i);
    }
    return temp_acc[pid];
}

void run_thread(int pid) {
    int r;
    int start = pid*MIN_WORKLOAD;
    for (int offset = start; offset < start + (workload[pid] * MIN_WORKLOAD); offset += MIN_WORKLOAD) {
        r = setjmp(*bufs[pid]);
        thread_acc[pid] += run_work_unit(pid,offset);
        temp_acc[pid] = 0.0;
        longjmp(sched,SUSPENDED);//sometimes
    }
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
            longjmp(*bufs[pid],RUNNING);
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
}


// void run_expropriative(){
//     int r;
//     while(!threads_done()){
//         int to_run = choose_winner();
//         r = setjmp(sched);
//         if(r==0) call_thread(to_run);
//         else {//Returning from thread.
//             if(r==TIMEOUT){ //triggered by timeout func.
//                 thread_status[to_run] = SUSPENDED;    
//             } else { //triggered by thread notification
//                 thread_status[to_run] = r;
//                 if(thread_status[to_run] == DONE){
//                    remove_tickets(to_run); 
//                 }
//             }
//         }
//     }
// }


// void do_timeout(){
//     longjmp(sched,TIMEOUT);
// }

int main(int argc, char** argv){
    //Initialize running environment.
    struct Property property;
    initProperty(&property);
    initialize_global(&property);

    run_non_expropriative();
    

    for(int i=0;i<10;i++){
        printf("Value %d times: %d\n",i,cant[i]);
    }
    return 0;
}



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