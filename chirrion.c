#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

jmp_buf sched,bufferA,bufferB;
int NOT_STARTED=0;
int RUNNING=1;
int SUSPENDED=2;
int DONE=3;
struct Thread;
typedef struct Thread{
	volatile int pid;
	void (*f_p)(struct Thread*);
	jmp_buf* context;
	volatile int status;
	volatile int times;
}  Thread;

typedef struct Scheduler {
	volatile Thread* threads;
	int numThreads;
} Scheduler;
Scheduler s;

int turn = 0;

Thread* chooseThread(Scheduler s) {
	return &s.threads[turn=(turn+1)%2];//always choose number one for now.
}

void runOrResume(Thread* t) {
	if(t->status == NOT_STARTED){
		t->status = RUNNING;
		printf("Thread %d not started, calling func!\n",t->pid);
		t->f_p(t);
	} else {
		if(t->status != DONE) {
			t->status = RUNNING;
			longjmp(*t->context,RUNNING);
		}
	}
}

int someThreadAlive(Scheduler s){
	for (int i = 0; i < s.numThreads; ++i)
	{
		if(s.threads[i].status!=DONE)return 1;
	}
	return 0;
}

void schedule(Scheduler s) {
	int r;
	Thread* t;
	while(someThreadAlive(s)){
		t = chooseThread(s);
		r = setjmp(sched);
		if(r==0) runOrResume(t);
		else {
			printf("setting status of thread %d to %d\n",t->pid,r);
			//Do something if thread dies.
			t->status = r;
		}
	}
}

void printOne(Thread* t) {
	printf("Thread %d init\n",t->pid);
	int r;
	while(t->times--) {
		printf("iteration of thread %d: %d\n",t->pid,t->times);
		r = setjmp(*t->context);
		if(r==0) longjmp(sched,SUSPENDED);
		printf("Came back to %d, loops remaining %d\n",t->pid,t->times);
	}
	printf("Killing thread one\n");
	longjmp(sched,DONE);
}


int main(int argc, char **argv) 
{
	int x = 2;
	Thread* ta = malloc(sizeof(Thread)*x);
	ta[0].pid = 0;
	ta[1].pid = 1;
	ta[0].f_p = printOne;
	ta[1].f_p = printOne;
	ta[0].times = 10;
	ta[1].times = 10;
	ta[0].context = &bufferA;
	ta[1].context = &bufferB;
	ta[0].status = NOT_STARTED;
	ta[1].status = NOT_STARTED;
	// for (int i = 0; i < x; ++i)
	// {
	// 	ta[i].f_p = printOne;
	// }
	s.numThreads=x;
	s.threads = ta;
	schedule(s);
    return 0;
}
