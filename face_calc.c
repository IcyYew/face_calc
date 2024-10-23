#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FACE_MAX 2048





void add_to_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, 
				int n, int x, unsigned short seed, short num_runs, int result);
void dump_log(int *n_log, int *x_log, unsigned short *seed_log, int *summation_log, short num_runs);
int summation_of_faces(int x, int n);
void seed_gen(unsigned short* seed);

int main() {
	// top face of die
	int x;
	// number of total face on the die
	int n;
	
	int num_faces;
	
	int result;
	
	// mode selection
	int mode = -1;
	// running bool
	int running = 1;
	// number of runs this session
	//  note! decide if I want to keep this on a session basis or keep a running total when I implement logging of results between program runs
	short num_runs = 0;
	// seed for the random mode
	unsigned short seed;
	
	int x_log[100];
	int n_log[100];
	// note!!! need to set seed_log[num_runs] to 0 if random mode not selected
	unsigned short seed_log[100];
	int summation_log[100];
	
	
	
	printf("This program calculates the sum of all visible faces of a die.\n");
	printf("It features two modes, manual and random.\n");
	printf("Please input a face limit for the random mode, maximum of 2048: ");
	if (scanf("%d", &num_faces) > FACE_MAX) {
		printf("INPUT ERROR: Using default maximum of 2048\n");
		while(getchar() != '\n');
		num_faces = FACE_MAX;
	}
	
	// generate seed (or take input), seed random to the seed
	
	fflush(stdout);
	while (running) {
		if (num_runs > 0) {
			printf("Input 0 to exit the program, 1 to continue: ");
			scanf("%d", &running);
			if (running != 1) {
				break;
			}
		}
		x = 0;
		n = 0;
		result = 0;
		seed = 0;
		printf("Input 0 for manual mode, or 1 for random mode: ");
		scanf("%d", &mode);
		while (mode != 0 && mode != 1) {
			printf("Input 0 for manual mode, or 1 for random mode: ");
			scanf("%d", &mode);
		}
		switch(mode) {
			case(0):
				printf("\nManual mode selected!\n\tInput the number of faces on the die: ");
				scanf("%d", &n);
				printf("\tInput the number on the top of the die: ");
				scanf("%d", &x);
				result = summation_of_faces(x, n);
				add_to_log(n_log, x_log, seed_log, summation_log, n, x, 0, num_runs, result);
				printf("\n\tSummation of all visible faces for a %d sided die with %d on the top: %d\n", n, x, result); 
				break;
				
			case(1):
				seed_gen(&seed);
				srand(seed);
				printf("\nRandom mode selected!");
				printf("\n\tGenerating with seed: 0x%X ...", seed);
				n = rand() % num_faces + 1;
				x = rand() % n + 1;
				result = summation_of_faces(x, n);
				add_to_log(n_log, x_log, seed_log, summation_log, n, x, seed, num_runs, result);
				printf("\n\n\tSummation of all visible faces for a %d sided die with %d on the top: %d\n", n, x, result);
				break;
		}
		num_runs++;
	}
	dump_log(n_log, x_log, seed_log, summation_log, num_runs);
	printf("\n\nThanks for using!");
	return 0;
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
