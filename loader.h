#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_WORKLOAD 50
#define MODE_NO_EXPROPIATIVO 0
#define MODE_EXPROPIATIVO 1


struct Property {
	int *tickets;
	int *workload;
	int *workload_calc;
	int size;
	int max_lottery;
	int quantum;
	int mode;
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

void loadTickets(struct Property* t,char *file){
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
		//printf("%s",line);
		t->tickets[ret] = atoi(line);
		t->max_lottery += t->tickets[ret];
		ret++;
	}
	fclose(f);
    if (line)
        free(line);
}

void loadWorkload(struct Property* t,char *file){
	int i = 0;
	FILE * f = fopen(file, "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 0;
	if (f == NULL){
		printf("Error Abriendo Archivo");
        exit(EXIT_FAILURE); 
	}
	while ((read = getline(&line, &len, f)) != -1) {
		//printf("%s",line);
		t->workload[ret] = atoi(line);
		t->workload_calc[ret] = t->workload[ret]*MIN_WORKLOAD;
		ret++;
	}
	fclose(f);
    if (line)
        free(line);
}

void loadQuantum(struct Property* t,char *file){
	int i = 0;
	FILE * f = fopen(file, "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 0;
	if (f == NULL){
		printf("Error Abriendo Archivo");
        exit(EXIT_FAILURE); 
	}
	while ((read = getline(&line, &len, f)) != -1) {
		t->quantum = atoi(line);
	}
	fclose(f);
    if (line)
        free(line);
}

void loadMode(struct Property* t,char *file){
	int i = 0;
	FILE * f = fopen(file, "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 0;
	if (f == NULL){
		printf("Error Abriendo Archivo");
        exit(EXIT_FAILURE); 
	}
	while ((read = getline(&line, &len, f)) != -1) {
		t->mode = atoi(line);
	}
	fclose(f);
    if (line)
        free(line);
}

void initProperty(struct Property *property){
	property->size = countLines("tickets.txt");

	property->tickets = malloc(property->size*sizeof(int));
	property->workload = malloc(property->size*sizeof(int));
	property->workload_calc = malloc(property->size*sizeof(int));

	loadTickets(property, "tickets.txt");
	loadWorkload(property,"workload.txt");
	loadQuantum(property,"quantum.txt");
	loadMode(property,"mode.txt");

	printf("Lines: %d\n",property->size);

	for(int i = 0; i < property->size; i++){
		printf("[%d][%d][%d]\n", property->tickets[i],property->workload[i],property->workload_calc[i]);
	}

	printf("Max Lottery: %d\n",property->max_lottery);
	printf("Quantum: %d\n", property->quantum);
	printf("Mode: %d\n", property->mode);
}

/*
	struct Property property;
	initProperty(&property);
*/

#endif