#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#define NUM_PLANTS 100
// #define NUM_PLANTS 25
#define NUM_CONDITIONS 32
// #define NUM_CONDITIONS 14

#include "main.h"

typedef struct pot {
	int64_t idx;
	char plant;
	char plant_next;
	struct pot* next;
	struct pot* prev;
} pot_t;

typedef struct condition {
	char cond[5];
	char result;
} condition_t;

void print_pots(pot_t* pot) {
	if (pot && pot->plant) {
		printf("%d ", pot->plant);
		print_pots(pot->next);
	}
}

char get_plant(pot_t* pot) {
	if (pot) {
		if (pot->plant) {
			return pot->plant;
		} else {
			return '.';
		}
	}
}

char find_condition(pot_t* pot, condition_t* conditions) {
	char pot_state[5];

	pot_state[0] = get_plant(pot->prev->prev);
	pot_state[1] = get_plant(pot->prev);
	pot_state[2] = pot->plant;
	pot_state[3] = get_plant(pot->next);
	pot_state[4] = get_plant(pot->next->next);

	// printf("%.5s\n", pot_state);


	uint8_t matches = 0;
	for (int i = 0; i < NUM_CONDITIONS; i++) {
		matches = 0;
		for (int j = 0; j < 5; j++) {
			matches += pot_state[j] == conditions[i].cond[j];
		}
		if (matches == 5) { return conditions[i].result; }
	}
	return 0;
}

void grow(pot_t* left_pot, pot_t* right_pot) {
	if (left_pot->next->next->plant == '#') {
		pot_t* before = left_pot->next;
		pot_t* after = left_pot->next->next;

		pot_t* pot = malloc(sizeof(pot_t));
		pot->idx = after->idx - 1;
		pot->plant = '.';
		pot->prev = before;
		before->next = pot;
		pot->next = after;
		after->prev = pot;
		before->idx--;
	}

	if (right_pot->prev->prev->plant == '#') {
		pot_t* after = right_pot->prev;
		pot_t* before = right_pot->prev->prev;

		pot_t* pot = malloc(sizeof(pot_t));
		pot->idx = before->idx + 1;
		pot->plant = '.';
		pot->prev = before;
		before->next = pot;
		pot->next = after;
		after->prev = pot;
		after->idx++;
	}
}

int64_t calc_sum(pot_t* pot) {
	if (pot && pot->plant) {
		int32_t val = pot->plant == '#' ? pot->idx : 0;
		return val + calc_sum(pot->next);
	} else { return 0; }
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 12" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end;

	uint8_t state = 0;
	uint8_t pot_idx = 0;
	uint8_t idx = 0;
	uint8_t cond_idx = 0;
	char c;

	pot_t* left_pot = malloc(sizeof(pot_t));
	left_pot->idx = -1;
	left_pot->plant = 0;
	left_pot->next = NULL;
	left_pot->prev = NULL;

	pot_t* cur_pot = left_pot;

	condition_t conditions[NUM_CONDITIONS];

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	for (int i = 0; i < 2; i++) {
		pot_t* pot = malloc(sizeof(pot_t));
		pot->idx = i - 2;
		pot->plant = '.';
		pot->prev = cur_pot;
		pot->next = NULL;
		cur_pot->next = pot;
		cur_pot = pot;
	}

	while ((c = fgetc(fp)) != '\n' && c != EOF) {
		if (state < 2) {
			if (c == ' ') { state++; }
		} else {
			if (pot_idx < NUM_PLANTS) {
				pot_t* pot = malloc(sizeof(pot_t));
				pot->idx = pot_idx++;
				pot->plant = c;
				pot->prev = cur_pot;
				pot->next = NULL;
				cur_pot->next = pot;
				cur_pot = pot;
			}
		}
	}

	for (int i = 0; i < 2; i++) {
		pot_t* pot = malloc(sizeof(pot_t));
		pot->idx = pot_idx++;
		pot->plant = '.';
		pot->prev = cur_pot;
		pot->next = NULL;
		cur_pot->next = pot;
		cur_pot = pot;
	}

	pot_t* right_pot = malloc(sizeof(pot_t));
	right_pot->idx = -1;
	right_pot->plant = 0;
	right_pot->next = NULL;
	right_pot->prev = cur_pot;
	cur_pot->next = right_pot;

	print_pots(left_pot->next);
	printf("\n");

	while ((c = fgetc(fp)) == '\n') {};
	fseek(fp, -1, SEEK_CUR);

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		for (int i = 0; i < len; i++) {
			switch (state) {
				case 0:
					if (line[i] == ' ') {
						state++;
						idx = 0;
					} else { conditions[cond_idx].cond[idx++] = line[i]; }
					break;
				case 1:
					if (line[i] == ' ') { state++; }
					break;
				case 2:
					conditions[cond_idx].result = line[i];
					state++;
					break;
			}
		}

		printf("%.5s => %c\n", conditions[cond_idx].cond, conditions[cond_idx].result);
		cond_idx++;
	}

	char result = 0;
	pot_t* pot = NULL;

	int64_t generations = 2000;
	for (int g = 0; g < generations; g++) {
		pot = left_pot->next;
		while (pot->plant) {
			if (result = find_condition(pot, conditions)) {
				pot->plant_next = result;
			} else {
				pot->plant_next = pot->plant;
			}

			pot = pot->next;
		}

		// printf("start %d: ", g);
		// print_pots(left_pot->next);
		// printf("\n");

		pot = left_pot->next;
		while (pot->plant) {
			pot->plant = pot->plant_next;
			pot = pot->next;
		}

		grow(left_pot, right_pot);

		// printf("end %d:   ", g);
		// print_pots(left_pot->next);
		// printf("\n");

		int64_t sum = calc_sum(left_pot->next);
		printf("%ld\n", sum);
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
