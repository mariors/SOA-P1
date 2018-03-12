#include "loader.h"
#include "gtk_ui.h"
#include <stdio.h>
#include <math.h>

char **ids;



void * inc_x (void *x_void_ptr) {

	float max_work = 100;
	int threads = 5;
	int completed = 0;

	int work[5];
	work[0] = 0;
	work[1] = 0;
	work[2] = 0;
	work[3] = 0;
	work[4] = 0;


	float pi = 3.1415;

	float calc[5];
	calc[0] = pi;
	calc[1] = pi;
	calc[2] = pi;
	calc[3] = pi;
	calc[4] = pi;


	while (completed < threads) {

		for(int i = 0; i < threads; i++){
			usleep(20000);

			if(work[i] < max_work){

				work[i]++;
				int finish = 0;

				if(work[i] == max_work){
					finish = 1;
					completed++;
				}

				int act = 0;

				if(work[i] % 2 == 0){
					calc[i] += 0.1;
					act = 1;
				}else{
					calc[i] -= 0.1;
				}
				float perc = work[i]/max_work;

				update_row_status(i, perc, calc[i], act, finish);
			}
		}
	}

	return (NULL);
}


void expropiativo(){

	printf("EXP");

	pthread_t inc_x_thread;
	int t = 1;
	pthread_create (&inc_x_thread, NULL, inc_x, &t);

}

void no_expropiativo(){

	printf("NO_EXP");

	pthread_t inc_x_thread;
	int t = 1;
	pthread_create (&inc_x_thread, NULL, inc_x, &t);

}


int main(int argc, char * argv[]){

	struct Property property;
	initProperty(&property);

//	int nThreads = 5;
//	int mode = 0;


	ids = malloc (sizeof (char *) * property.size);

	/* Secure gtk */
	gdk_threads_init ();

	/* Obtain gtk's global lock */
	gdk_threads_enter ();
	
	gtk_init(NULL,NULL);
	
	create_UI(property.size, property.mode);

	int i = 0;
	while(i < property.size){
		int t_id = i + 1;
		int nDigits = numberOfDigits (t_id);
		char *id_char = malloc (sizeof (char) * (nDigits + 1));
		sprintf (id_char, "%d", t_id);
		ids[i] = id_char;
		add_row(t_id, id_char);
		i++;
	}

	show_ui();

	if(property.mode == 1){
		expropiativo();
	}else{
		no_expropiativo();
	}


	gtk_main();

	gdk_threads_leave ();

	return 0;
}
