#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

//Test for arrays to access things.
int NOT_STARTED=0;
int RUNNING=1;
int SUSPENDED=2;
int DONE=3;
int* volatile times;
void (**f_ps)(int,jmp_buf);
jmp_buf sched;

void printOne(volatile int pid,jmp_buf context) {
	printf("Thread %d init\n",pid);
	int r;
	while(times[pid]--) {
		printf("iteration of thread %d: %d\n",pid,times[pid]);
		r = setjmp(context);
		if(r==0) longjmp(sched,SUSPENDED);
		printf("Came back to %d, loops remaining %d\n",pid,times[pid]);
		// times--;
	}
	printf("Killing thread %d\n",pid);
	longjmp(sched,DONE);
}


int main(){
	int numThreads = 2;
	times = malloc(sizeof(int)*numThreads);
	f_ps = malloc(sizeof(void(int,jmp_buf)) + sizeof(jmp_buf));
	for (int i = 0; i < numThreads; ++i)
	{
		times[i]=10;
		f_ps[i]=*printOne;
	}
	jmp_buf a,b;
	int r;
	int turn =0;
	while(times[0]!=0 && times[1] !=0){
		r=setjmp(sched);
		if(r==0)f_ps[turn](turn,a);
		else {printf("came back\n");}
		turn = (turn+1)%2;
	}
	printf("done\n");


	return 0;
}


