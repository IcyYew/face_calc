#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FACE_MAX 2048
#define ATTEMPTS_MAX 5

/*
//TESTING VARS
#define MAX_ALLOCATIONS 2
*/

typedef struct {
	int *n_log;
	int *x_log;
	unsigned short *seed_log;
	int *summation_log;
	short num_runs;
	short alloc_cap;
} DataLog;

// Initialize the data log struct
DataLog* init_data_log(short alloc_cap);
// Boilerplate handler for reallocation fail
void realloc_fail(DataLog *log, size_t size_request);
// Boilerplate handler for malloc fail, no parameters needed since this can only happen at initialization
void malloc_fail(size_t size_request);
// add content to the log
void add_to_log(DataLog *log, int n, int x, unsigned short seed, int result);
// free all allocated memory in the log
void free_memory(DataLog *log);
// expand the log to allocate more memory if we run out of space
void expand_data_log(DataLog *log);
// dump the entire log
void dump_log(DataLog *log);

/*
// testing methods for realloc fail
int num_allocations = 0;
void *realloc_fail_test(void *ptr, size_t size);


void expand_data_log_m(DataLog *log);
*/

int mode_input();
// calculation to determine the sum of all visible faces
int summation_of_faces(int x, int n);
//generate or take input of a 4 character hexadecimal sequence i.e. 0xABCE
void seed_gen(unsigned short* seed);

int main() {
	int n, x, result;
	short initial_capacity = 5;
	DataLog *log = init_data_log(initial_capacity);
	int max_num_faces, mode = -1, running = 1, mode_lock = 0;
	unsigned short seed;
	
	printf("This program calculates the sum of all visible faces of a die.\n");
	printf("It features two modes, manual and random.\n");
	printf("Please input a face limit for the random mode, maximum of 2048: ");
	
	if (scanf("%d", &max_num_faces) != 1) {
		printf("INPUT ERROR: Invalid input. Using default maximum of 2048\n");
		while(getchar() != '\n');
		max_num_faces = FACE_MAX;
	}
	else if (max_num_faces > FACE_MAX) {
		printf("INPUT ERROR: Maximum limit exceeded. Using default maximum of 2048\n");
		max_num_faces = FACE_MAX;
	}
	
	
	fflush(stdout);
	while (running) {
		if (log->num_runs > 0) {
			printf("Input 0 to exit the program, 1 to continue: ");
			if (scanf("%d", &running) != 1) {
				printf("INPUT ERROR: Invalid input. Dumping log and exiting.\n");
				while (getchar() != '\n');
				break;
			}
			if (running == 0) {
				break;
			}
		}
		x = 0;
		n = 0;
		result = 0;
		seed = 0;
		
		if (log->num_runs == 0) {
			mode = mode_input(ATTEMPTS_MAX);
			if (mode == -1) {
				dump_log(log);
				exit(0);
			}
			else if (mode == 2) {
				mode = 0;
				mode_lock = 1;
			}
			else if (mode == 3) {
				mode = 1;
				mode_lock = 1;
			}
		}
		if (!mode_lock && log->num_runs != 0) {
			mode = mode_input(ATTEMPTS_MAX);
			if (mode == -1) {
				dump_log(log);
				exit(0);
			}
			else if (mode == 2) {
				mode = 0;
				mode_lock = 1;
			}
			else if (mode == 3) {
				mode = 1;
				mode_lock = 1;
			}
		}
		
		switch(mode) {
			case(0):
				int attempts = 0;
				printf("\nManual mode selected!\n\tInput the number of faces on the die: ");
				scanf("%d", &n);
				printf("\tInput the number on the top of the die: ");
				while (attempts < ATTEMPTS_MAX) {
					if (scanf("%d", &x) == 1 && x < n) {
						break;
					}
					else {
						attempts++;
						printf("Invalid input, you have %d attempts left", ATTEMPTS_MAX - attempts);
						while (getchar() != '\n');
						if (attempts < ATTEMPTS_MAX) {
							printf("\tInput the number on the top of the die: ");
						}
						else if (attempts == ATTEMPTS_MAX) {
							printf("\nToo many attempts. Exiting program and dumping log.\n\n");
							dump_log(log);
							exit(0);
						}
						
					}
				}
				
				result = summation_of_faces(x, n);
				add_to_log(log, n, x, 0, result);
				printf("\n\tSummation of all visible faces for a %d sided die with %d on the top: %d\n", n, x, result); 
				break;
				
			case(1):
				seed_gen(&seed);
				srand(seed);
				printf("\nRandom mode selected!");
				printf("\n\tGenerating with seed: 0x%X ...", seed);
				n = rand() % max_num_faces + 1;
				x = rand() % n + 1;
				result = summation_of_faces(x, n);
				add_to_log(log, n, x, seed, result);
				printf("\n\n\tSummation of all visible faces for a %d sided die with %d on the top: %d\n", n, x, result);
				break;
		}
	}
	if (log->num_runs == 0) {
		free_memory(log);
		printf("\n\nThanks for using!");
		return 0;
	}
	else {
		dump_log(log);
		free_memory(log);
		printf("\n\nThanks for using!");
		return 0;
	}
}

/* *******************************************************************
   *						Regular Functions                        *
   ******************************************************************* */

void seed_gen(unsigned short* seed) {
	srand(time(NULL));
	int gen_temp = -1;
	
	printf("If you would like to use a custom seed for the random mode, enter 1, anything else to continue: ");
	if (scanf("%d", &gen_temp) != 1) {
		while (getchar() != '\n'); // clear buffer
		gen_temp = 0; // go random
	}
	if (gen_temp == 1) {
		printf("Input a four digit hexadecimal in the format, 0xFFFF: ");
		scanf("%hx", seed);
	}
	else {
		for (int i = 0; i < 4; i++) {
			unsigned short hex_temp = rand() % 16;
			*seed = (*seed << (unsigned short) 4) | hex_temp;
		}
	}
}

int summation_of_faces(int x, int n) {
	int p1 = (n * (n + 1)) / 2;
	return p1 - n - 1 + x;
}

int mode_input() {
	int mode = -1;
	int attempts = 0;
	
	printf("Input 0 for manual mode, or 1 for random mode...\n");
	printf("Alternatively, input 2 for permanent manual mode, or 3 for permanent random mode: ");
	while (attempts < ATTEMPTS_MAX) {
		if (scanf("%d", &mode) == 1 && (mode == 0 || mode == 1 || mode == 2 || mode == 3)) {
			return mode;
		}
		else {
			attempts++;
			printf("Invalid input, you have %d attempts left\n", ATTEMPTS_MAX - attempts);
			while (getchar() != '\n');
			if (attempts < ATTEMPTS_MAX) {
				printf("Input 0 for manual mode, or 1 for random mode...\n");
				printf("Alternatively, input 2 for permanent manual mode, or 3 for permanent random mode: ");
			}
		}
		
	}
	printf("Too many attempts. Exiting program and dumping log.\n");
	return -1;
}


/* *******************************************************************
   *						Logging Functions                        *
   ******************************************************************* */
void dump_log(DataLog *log) {
	printf("Log Dump:\n");
	printf("-----------------------------------------------------------------\n");
	printf("| Run # | Faces   | Top Face | Seed              | Result         |\n");
	printf("-----------------------------------------------------------------\n");
	
	for (int i = 0; i < log->num_runs; i++) {
		if (log->seed_log[i] == 0) {
			printf("| %-5d | %-7d | %-8d | %-14s | %-14d |\n", i + 1, log->n_log[i], log->x_log[i], "no seed or 0x0000", log->summation_log[i]);  
		}
		else {
			printf("| %-5d | %-7d | %-8d | 0x%-12X    | %-14d |\n", i + 1, log->n_log[i], log->x_log[i], log->seed_log[i], log->summation_log[i]);
		}
	}
	printf("-----------------------------------------------------------------\n");
}

void free_memory(DataLog *log) {
	free(log->n_log);
	log->n_log = NULL;
	free(log->x_log);
	log->x_log = NULL;
	free(log->seed_log);
	log->seed_log = NULL;
	free(log->summation_log);
	log->summation_log = NULL;
	free(log);
	log = NULL;
	
}

void add_to_log(DataLog *log, int n, int x, unsigned short seed, int result) {
	short tmp_runs = log->num_runs;
	if (tmp_runs >= log->alloc_cap) {
		expand_data_log(log);
	}
	log->n_log[tmp_runs] = n;
	log->x_log[tmp_runs] = x;
	log->seed_log[tmp_runs] = seed;
	log->summation_log[tmp_runs] = result;
	log->num_runs++;
}

DataLog* init_data_log(short alloc_cap) {
	DataLog *log = (DataLog*) malloc(sizeof(DataLog));
	log->n_log = (int*) malloc(sizeof(int) * alloc_cap);
	if (log->n_log == NULL) {
		malloc_fail(sizeof(int) * alloc_cap);
	}
	log->x_log = (int*) malloc(sizeof(int) * alloc_cap);
	if (log->x_log == NULL) {
		malloc_fail(sizeof(int) * alloc_cap);
	}
	log->seed_log = (unsigned short*) malloc(sizeof( unsigned short) * alloc_cap);
	if (log->seed_log == NULL) {
		malloc_fail(sizeof(unsigned short) * alloc_cap);
	}
	log->summation_log = (int*) malloc(sizeof(int) * alloc_cap);
	if(log->summation_log == NULL) {
		malloc_fail(sizeof(int) * alloc_cap);
	}
	log->num_runs = 0;
	log->alloc_cap = alloc_cap;
	return log;
}

void malloc_fail(size_t size_request) {
	fprintf(stderr, "ERROR: malloc() fail! Requested size: %zu bytes\n", size_request);
	fprintf(stderr, "SERIOUS FAILURE, INSPECT YOUR ENVIRONMENT AND/OR SYSTEM!\n");
	exit(EXIT_FAILURE);
}

void expand_data_log(DataLog *log) {
	printf("\n\n\t\t\tEXPANDING LOG ALLOCATION\n\n");
	log->alloc_cap *= 1.5;
	
	int *tmp_n_log = (int*) realloc(log->n_log, sizeof(int) * log->alloc_cap);
	if (tmp_n_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->n_log = tmp_n_log;
	
	int *tmp_x_log = (int*) realloc(log->x_log, sizeof(int) * log->alloc_cap);
	if (tmp_x_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->x_log = tmp_x_log;
	
	unsigned short *tmp_seed_log = (unsigned short*) realloc(log->seed_log, sizeof(unsigned short) * log->alloc_cap);
	if (tmp_seed_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->seed_log = tmp_seed_log;
	
	int *tmp_summation_log = (int*) realloc(log->summation_log, sizeof(int) * log->alloc_cap);
	if (tmp_summation_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->summation_log = tmp_summation_log;
}

void realloc_fail(DataLog *log, size_t size_request) {
	fprintf(stderr, "ERROR: realloc() fail! Requested new size: %zu bytes.\n", size_request);
	dump_log(log);
	exit(EXIT_FAILURE);
}

/*
void *realloc_fail_test(void *ptr, size_t size) {
	num_allocations++;
	if (num_allocations > MAX_ALLOCATIONS) {
		return NULL;
	}
	return realloc(ptr, size);
}

void expand_data_log_m(DataLog *log) {
	printf("\n\n\t\t\tEXPANDING LOG ALLOCATION\n\n");
	log->alloc_cap *= 1.5;
	
	int *tmp_n_log = (int*) realloc_fail_test(log->n_log, sizeof(int) * log->alloc_cap);
	if (tmp_n_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->n_log = tmp_n_log;
	
	int *tmp_x_log = (int*) realloc_fail_test(log->n_log, sizeof(int) * log->alloc_cap);
	if (tmp_x_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->x_log = tmp_x_log;
	
	unsigned short *tmp_seed_log = (unsigned short*) realloc_fail_test(log->seed_log, sizeof(unsigned short) * log->alloc_cap);
	if (tmp_seed_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->seed_log = tmp_seed_log;
	
	int *tmp_summation_log = (int*) realloc_fail_test(log->n_log, sizeof(int) * log->alloc_cap);
	if (tmp_summation_log == NULL) {
		realloc_fail(log, sizeof(int) * log->alloc_cap);
	}
	log->summation_log = tmp_summation_log;
}*/
