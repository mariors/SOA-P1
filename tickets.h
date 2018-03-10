#ifndef TICKETS_H
#define TICKETS_H

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>


struct Tickets {
	int *list;
	int size;
	int max_lottery;
};

int countLines(char *file){
	FILE * f = fopen(file, "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 0;
	if (f == NULL){
		printf("Error Abriendo Archivo");
        exit(EXIT_FAILURE); 
	}
	while ((read = getline(&line, &len, f)) != -1) {ret++;}
	fclose(f);
    if (line)
        free(line);
    return ret;
}

void readTickets(struct Tickets* t,char *file){
	int i = 0;
	FILE * f = fopen(file, "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 0;
    t->max_lottery = 0;
	if (f == NULL){
		printf("Error Abriendo Archivo");
        exit(EXIT_FAILURE); 
	}
	while ((read = getline(&line, &len, f)) != -1) {
		printf("%s",line);
		t->list[ret] = atoi(line);
		t->max_lottery += t->list[ret];
		ret++;
	}
	fclose(f);
    if (line)
        free(line);
}

void loadTickets(struct Tickets* t,char *file){
	t->size = countLines(file);
	t->list = malloc(t->size*sizeof(int));
	readTickets(t,file);
}

/*

	struct Tickets tickets;
	loadTickets(&tickets, "tickets.txt");

	printf("Lines: %d\n",tickets.size);

	for(int i = 0; i < tickets.size; i++){
		printf("[%d]\n", tickets.list[i]);
	}

	printf("Max Lottery: %d\n",tickets.max_lottery);

*/
#endif