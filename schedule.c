

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include "loader.h"
//#include "arctan_func.h"

#define MAX_THREADS 100
#define NOT_STARTED 0
#define RUNNING 1
#define SUSPENDED 2
#define DONE 3
#define TIMEOUT 4

int L = 100;

long double arctan_aproximation(int x, int iter){
    return 2 * (4*L * (x/ (pow(2*iter-1,2)*pow(x,2)+(4*pow(L,2)))));
}

int num_tickets;

int num_threads;
int quantum;
int expropriative_mode;
int* cant;
int* tickets;
int* ticket_sum;
int* thread_status;
int* workload;
int* workit;
volatile int current_pid;
volatile int interrupted = 0;

//Series calculation.
long double* temp_acc;
long double* thread_acc;
// jmp_buf** bufs;
sigjmp_buf bufs[5];
sigjmp_buf sched;

int select_ticket(int);
void calculate_ticket_sum();
void remove_tickets(int);
int choose_winner();
long double run_work_unit(int,int);
void run_thread();

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
    workit = malloc(sizeof(int)*num_threads);
    // bufs = malloc(sizeof(jmp_buf)*num_threads);
    workload = property->workload;
    for (int i = 0; i < num_threads; ++i) {
        jmp_buf aux;
        cant[i]=0;
        ticket_sum[i]=0;
        thread_status[i]=NOT_STARTED;
        temp_acc[i] = 0.0;
        thread_acc[i] = 0.0;
        workit[i] = 1;
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
            if(ticket_sum[i] > winner){
                printf("Winner! %d\n",i);
                return i;
            }
        }
    
    return -1;
}

long double calculate_series_element(int iter) {return 1.0;}

void run_thread() {
    printf("Running %d quantum %d\n",current_pid,quantum);
    int r;
    // int r = sigsetjmp(bufs[current_pid],1);
    // printf("Yield rate: %d on quantum %d\n",yield_rate,quantum);
    printf("Running %d\n",current_pid);
    for (; workit[current_pid] < (workload[current_pid] * MIN_WORKLOAD); ++workit[current_pid]) {
        // printf("Starting iteration %d\n",current_pid);
        printf("inside loop %d\n",current_pid);

        printf("%d\n",workit[current_pid]);
        thread_acc[current_pid] += arctan_aproximation(1,workit[current_pid]);
        // printf("(Mode %d)PID %d it %d/%d: %Lf\n",expropriative_mode,current_pid,workit[current_pid],(workload[current_pid] * MIN_WORKLOAD)+1,thread_acc[current_pid]);
        printf("Calculated one step %d\n",current_pid);
        // temp_acc[current_pid] = 0.0;
        //update_row_status(current_pid,1.0 * workit[current_pid]/(workload[current_pid] * MIN_WORKLOAD),thread_acc[current_pid],1,0); 
        
        printf("%d %d %d %d %d %d\n",
            quantum,
            current_pid, 
            workload[current_pid],
            (workload[current_pid] * MIN_WORKLOAD)
            ,(quantum*((workload[current_pid] * MIN_WORKLOAD)+1)),
            (int)(quantum/100.0 * (workload[current_pid] * MIN_WORKLOAD))+1);
        if(
            (workit[current_pid] >0 
            && workit[current_pid] % (int)(quantum/100.0 * (workload[current_pid] * MIN_WORKLOAD)) == 0 
            && workit[current_pid] % MIN_WORKLOAD==0 
            && expropriative_mode==MODE_NO_EXPROPIATIVO) 
            || interrupted){
            update_row_status(current_pid,1.0 * workit[current_pid]/(workload[current_pid] * MIN_WORKLOAD),thread_acc[current_pid],0,0); 
            printf("PID %d yielding\n",current_pid);
            int exception = SUSPENDED;
            if(interrupted) {
                printf("PID %d yielded because of interrupt\n",current_pid);
                exception = TIMEOUT;
                interrupted=0;
            }
            r = sigsetjmp(bufs[current_pid],1);
            if(r==0) siglongjmp(sched,exception);
            else {printf("PID %d came back from scheduler\n",current_pid);
        }
        }
    }
    printf("Thread %d done \n",current_pid);
    // printf("Final value of thread %d: %Lf\n",pid,thread_acc[pid]);
    // r = sigsetjmp(bufs[pid],1);
    update_row_status(current_pid,1,thread_acc[current_pid],0,1); 
    siglongjmp(sched,DONE);
}

int threads_done() {
    for (int i = 0; i < num_threads; ++i){
        if(thread_status[i]!=DONE) {
            printf("Thread %d not done\n",i);
            return 0;
        }
    }
    return 1;
}

void call_thread(int pid) {
    printf("Calling to run: %d\n",pid);
    if(thread_status[pid] == NOT_STARTED){
        thread_status[pid] = RUNNING;
        // printf("Thread %d not started, calling func!\n",pid);

        //ASsign workload to thread
        current_pid = pid;
        run_thread();
    } else {
        if(thread_status[pid] != DONE) {
            printf("Resuming: %d\n",pid);
            thread_status[pid] = RUNNING;
            printf("Calling signlongjmp resume pid %d\n",pid);
            current_pid = pid;
            siglongjmp(bufs[pid],RUNNING);
        } else {
            // printf("Thread %d already done \n",pid);
        }
    }
}


void * run_non_expropriative(){
    int r;
    int latest = choose_winner();
    while(!threads_done()){
        printf("Scheduling loop\n");
        int to_run = choose_winner();
        // if(to_run == latest)continue;
        r = sigsetjmp(sched,1);
        if(r==0) call_thread(to_run);
        else {//Returning from thread.
            // printf("Returning from thread %d with status %d\n",to_run,r);
            thread_status[to_run] = r;
            latest = to_run;
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
        if(r==0) call_thread(to_run);
        else {//Returning from thread.
            printf("Exception code: %d\n",r);
            if(r==TIMEOUT){ //triggered by timeout func.
                printf("Came back from timeout %d\n",r);
                thread_status[to_run] = SUSPENDED;    
            } else { //triggered by thread notification
                thread_status[to_run] = r;
                if(thread_status[to_run] == DONE){
                   remove_tickets(to_run); 
                }
            }
        }
    }
     for (int i = 0; i < num_threads; ++i){
        printf("PID: %d Result: %Lf\n",i,thread_acc[i]);
    }
}

/*
void do_timeout(){
     siglongjmp(sched,TIMEOUT);
}*/

// int main(int argc, char** argv){
//     //Initialize running environment.
//     printf("Running no-expropriative\n");
//     struct Property property;
//     initProperty(&property);
//     initialize_global(&property);

//     run_non_expropriative();
//     return 0;
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