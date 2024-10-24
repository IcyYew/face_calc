#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FACE_MAX 2048
#define ATTEMPTS_MAX 5



int mode_input();

// add information about a run to the "log", maximum of 100 for this iteration of the program's purposes
void add_to_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, 
				int n, int x, unsinpgned short seed, short num_runs, int result);
				

// dump the entire log in pretty format				
void dump_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, short num_runs);

// calculation to determine the sum of all visible faces
int summation_of_faces(int x, int n);

//generate or take input of a 4 character hexadecimal sequence i.e. 0xABCE
void seed_gen(unsigned short* seed);

int main() {
	int x, n, result, max_num_faces, mode = -1, running = 1, mode_lock = 0;
	
	short num_runs = 0;
	
	unsigned short seed;
	
	int x_log[100], n_log[100], summation_log[100];
	// stores seed for each run, 0 if not a random mode run
	unsigned short seed_log[100];
	
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
		if (num_runs > 0) {
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
		
		if (num_runs == 0) {
			mode = mode_input(ATTEMPTS_MAX);
			if (mode == -1) {
				dump_log(n_log, x_log, seed_log, summation_log, num_runs);
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
		if (!mode_lock && num_runs != 0) {
			mode = mode_input(ATTEMPTS_MAX);
			if (mode == -1) {
				dump_log(n_log, x_log, seed_log, summation_log, num_runs);
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
							dump_log(n_log, x_log, seed_log, summation_log, num_runs);
							exit(0);
						}
						
					}
				}
				
				result = summation_of_faces(x, n);
				add_to_log(n_log, x_log, seed_log, summation_log, n, x, 0, num_runs, result);
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
				add_to_log(n_log, x_log, seed_log, summation_log, n, x, seed, num_runs, result);
				printf("\n\n\tSummation of all visible faces for a %d sided die with %d on the top: %d\n", n, x, result);
				break;
		}
		num_runs++;
	}
	if (num_runs == 0) {
		printf("\n\nThanks for using!");
		return 0;
	}
	else {
		dump_log(n_log, x_log, seed_log, summation_log, num_runs);
		printf("\n\nThanks for using!");
		return 0;
	}
}

void dump_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, short num_runs) {
	printf("Log Dump:\n");
	printf("-----------------------------------------------------------------\n");
	printf("| Run # | Faces   | Top Face | Seed              | Result         |\n");
	printf("-----------------------------------------------------------------\n");
	
	for (int i = 0; i < num_runs; i++) {
		if (seed_log[i] == 0) {
			printf("| %-5d | %-7d | %-8d | %-14s | %-14d |\n", i + 1, n_log[i], x_log[i], "no seed or 0x0000", summation_log[i]);  
		}
		else {
			printf("| %-5d | %-7d | %-8d | 0x%-12X    | %-14d |\n", i + 1, n_log[i], x_log[i], seed_log[i], summation_log[i]);
		}
	}
	printf("-----------------------------------------------------------------\n");
}

void add_to_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, 
				int n, int x, unsigned short seed, short num_runs, int result) {
	n_log[num_runs] = n;
	x_log[num_runs] = x;
	seed_log[num_runs] = seed;
	summation_log[num_runs] = result;
}

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
