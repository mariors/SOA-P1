#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>

#include "gtk_ui.h"
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
int cant[MAX_THREADS];
int tickets[MAX_THREADS];
int ticket_sum[MAX_THREADS];
int thread_status[MAX_THREADS];
int workload[MAX_THREADS];

//Series calculation.
long double temp_acc[MAX_THREADS];
long double thread_acc[MAX_THREADS];
// jmp_buf** bufs;
jmp_buf bufs[MAX_THREADS];
jmp_buf sched;

int select_ticket(int);
void calculate_ticket_sum();
void remove_tickets(int);
int choose_winner();
long double run_work_unit(int,int);
void run_thread(volatile int pid);

void initialize_global(struct Property* property) {
    num_threads = property->size;
    expropriative_mode = property->mode;
    // bufs = malloc(sizeof(jmp_buf)*num_threads);
    quantum = property->quantum;
    for (int i = 0; i < num_threads; ++i) {
        jmp_buf aux;
        cant[i]=0;
        ticket_sum[i]=0;
        thread_status[i]=NOT_STARTED;
        temp_acc[i] = 0.0;
        thread_acc[i] = 0.0;

        tickets[i] = property->tickets[i];
        workload[i] = property->workload[i];
    


        // bufs[i] = malloc(sizeof(jmp_buf));
        // bufs[i] = &aux;
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
        update_row_status(pid, 1.0 * workit/total_iterations,thread_acc[pid],1,0);
        if(workit >0 && workit % yield_rate == 0 && workit%MIN_WORKLOAD==0 && expropriative_mode == 0) {
            // printf("enough steps to yield\n");
            update_row_status(pid,1.0 * workit/total_iterations,thread_acc[pid],0,0);
            r = sigsetjmp(bufs[pid],1);
            if(r==0) { 
                siglongjmp(sched,SUSPENDED);
            }
            else {
                printf("came back!");            
                continue;
            }
        }
    }
    // printf("Thread %d done \n",pid);
    // printf("Final value of thread %d: %Lf\n",pid,thread_acc[pid]);
    // r = sigsetjmp(bufs[pid]);

    update_row_status(pid,1,thread_acc[pid],0,1);
    siglongjmp(sched,DONE);
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
        //Assign workload to thread
        run_thread(pid);
    } else {
        if(thread_status[pid] != DONE) {
            printf("Resuming: %d\n",pid);
            thread_status[pid] = RUNNING;
            printf("Calling lonjmp resume\n");
            int r = sigsetjmp(sched,1);
            if(r == 0) siglongjmp(bufs[pid],RUNNING);
            else{
                thread_status[pid] = r;
                if(thread_status[pid] == DONE){
                    remove_tickets(pid); 
                }
            }
        } else {
            printf("Thread %d already done \n",pid);
        }
    }
}


void * run_non_expropriative(){
    int r;
    printf("enter");
    while(!threads_done()){
        int to_run = choose_winner();
        r = sigsetjmp(sched,1);
        if(r==0) call_thread(to_run);
        else {//Returning from thread.
            thread_status[to_run] = r;
            if(thread_status[to_run] == DONE){
               remove_tickets(to_run); 
            }
        }
    }
    for (int i = 0; i < num_threads; ++i)
    {
        printf("PID: %d Result: %Lf\n",i,thread_acc[i]);
    }

}


void run_expropriative(){
    int r;
    while(!threads_done()){
        int to_run = choose_winner();
        r = sigsetjmp(sched,1);
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


// void do_timeout(){
//     siglongjmp(sched,TIMEOUT);
// }


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