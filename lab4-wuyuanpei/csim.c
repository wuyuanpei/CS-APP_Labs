/**
 * Name: Richard Wu
 * WUSTL ID: 464493
 * ClassL CSE361
**/

#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// argument inputs
int s = 0;
int E = 0;
int b = 0;
FILE * file;
char en = 0;

// simulate cache
long * * tags;
char * * vs;
long * * lru;

// store hit, miss, eviction
long hit = 0l;
long miss = 0l;
long eviction = 0l;

void change_lru(long s_num,int idx){
	for(int i = 0; i < E; ++i){
		if(lru[s_num][i] == -1){
			lru[s_num][i] = 1;
		}else if(lru[s_num][i] < idx && lru[s_num][i]){
			lru[s_num][i]++;
		}
	}
}

int max_lru(long s_num){
	int idx = 0;
	long max_lru = 0;
	for(int i = 0; i < E; ++i){
		if(lru[s_num][i] == 0)
			return i;
		if(lru[s_num][i] > max_lru){
			max_lru = lru[s_num][i];
			idx = i;
		}
	}
	return idx;
}

void move_cache(long addr){
	long tag = ((unsigned long)addr) >> (s + b);
	long s_num = (((unsigned long)addr) << (64 - s - b)) >> (64 - s);
	for(int i = 0; i < E; ++i){
		if(vs[s_num][i] == 1 && tags[s_num][i] == tag) {
			hit ++;
			if(en)
				printf(" hit");
			int idx = lru[s_num][i];
			lru[s_num][i] = -1;
			change_lru(s_num, idx);
			break;
		}
		if(i == E - 1){
			miss ++;
			if(en)
				printf(" miss");
			int max = max_lru(s_num);
			if(vs[s_num][max] == 0){
				lru[s_num][max] = -1;
				change_lru(s_num, 0x7fffffff);
			}else{
				eviction ++;
				if(en)
					printf(" eviction");
				int idx = lru[s_num][max];
				lru[s_num][max] = -1;
				change_lru(s_num, idx);
			}
			vs[s_num][max] = 1;
			tags[s_num][max] = tag;
		}
	}
}

void parse_file() {
	char buf[32];
	if (file == NULL) {
		return;
	}
	while (fgets(buf, sizeof(buf), file) != NULL) {
		if(buf[0] == 'I')
			continue;
		long addr_l = strtol(buf+3,NULL,16);
		if(buf[1] == 'L' || buf[1] == 'S'){
			 if(en){
				printf("L/S %lx ",addr_l);
			 }
			 move_cache(addr_l);
			 if(en)
				printf("\n");
		}
		if(buf[1] == 'M'){
			 if(en)
				printf("M %lx",addr_l);
			 move_cache(addr_l);
			 move_cache(addr_l);
			 if(en)
				printf("\n");
		}
	}
}

void free_memory() {
	int S = pow(2,s);
	for(int i = 0; i < S; ++i)  
		free(tags[i]);
    free(tags);
    for(int i = 0; i < S; ++i)  
		free(lru[i]);
    free(lru);
    for(int i = 0; i < S; ++i)  
		free(vs[i]);
    free(vs); 
}

void malloc_memory() {
	int S = pow(2,s);
	tags = (long * *)malloc(sizeof(long *) * S); 
	lru = (long * *)malloc(sizeof(long *) * S);  
    for(int i = 0; i < S; ++i)  
		tags[i]=(long *)malloc(sizeof(long) * E);
	for(int i = 0; i < S; ++i)  
		lru[i]=(long *)malloc(sizeof(long) * E);
	vs = (char * *)malloc(sizeof(char *) * S);  
    for(int i = 0; i < S; ++i)  
		vs[i]=(char *)malloc(sizeof(char) * E);
}

void parse_arg(int argc, char * argv[]) {
	for(int i = 1; i < argc; ++i) {
		if(!strcmp(argv[i],"-s") && i < argc-1) {
			s = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-E") && i < argc-1) {
			E = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-b") && i < argc-1) {
			b = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-t") && i < argc-1) {
			file = fopen(argv[i+1], "r");
        }
        if(!strcmp(argv[i],"-v")) {
			en = 1;
        }
	}
}

int main(int argc, char * argv[]) {
    parse_arg(argc, argv);
    malloc_memory();
    parse_file();
    free_memory();
    printSummary(hit, miss, eviction);
    return 0;
}
