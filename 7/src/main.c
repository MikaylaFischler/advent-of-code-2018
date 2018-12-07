#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

typedef struct step {
	uint8_t done;
	int8_t num_depends;
	uint8_t depends[26];
} step_t;

typedef struct queued_step {
	uint8_t done;
	int8_t timer;
	uint8_t to_remove;
	struct queued_step* next;
} queued_step_t;

void init_queue(queued_step_t* queue) {
	for (int i = 0; i < 5; i++) {
		queue[i].done = 1;
		queue[i].timer = -1;
		queue[i].to_remove = 0;
	}
}

void enqueue(queued_step_t* queue, uint8_t time, uint8_t to_remove) {
	for (int i = 0; i < 5; i++) {
		if (queue[i].done) {
			printf("enqueued\n");
			queue[i].done = 0;
			queue[i].timer = time;
			queue[i].to_remove = to_remove;
			break;
		}
	}
}

void dequeue(queued_step_t* queue, uint8_t i) {
	queue[i].done = 1;
	queue[i].timer = -1;
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 7" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }
	uint8_t idx = 0;

	// timing
	clock_t time_start, time_end;

	// string temp vars
	char dependency_in = 0x0;
	char step_in = 0x0;

	uint8_t state = 0;

	step_t* steps = calloc(sizeof(step_t), 26);

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		for (int i = 0; i < len; i++) {
			if (state == 0) {
				if (line[i] == ' ') { state++; }
			} else if (state == 1) {
				dependency_in = line[i];
				state++;
			} else if (state < 8) {
				if (line[i] == ' ') { state++; }
			} else if (state == 8) {
				step_in = line[i];
				break;
			}
		}

		steps[step_in - 0x41].depends[dependency_in - 0x41] = 1;
		steps[step_in - 0x41].num_depends++;
	}

	uint8_t completed = 0;
	uint8_t to_remove = 0;
	uint8_t available = 5;
	uint16_t t = 0;

	// PART 1
	// while (completed != 26) {
	// 	for (int i = 0; i < 26; i++) {
	// 		if (steps[i].num_depends == 0) {
	// 			steps[i].num_depends = -1;
	// 			to_remove = i;
	// 			printf("%c", i + 0x41);
	// 			completed++;
	// 			break;
	// 		}
	// 	}
	//
	// 	for (int j = 0; j < 26; j++) {
	// 		if (steps[j].depends[to_remove]) {
	// 			steps[j].depends[to_remove] = 0;
	// 			steps[j].num_depends--;
	// 		}
	// 	}
	// }

	// PART 2
	queued_step_t* queue = malloc(sizeof(queued_step_t) * 5);
	init_queue(queue);
	while (completed != 26) {
		for (int i = 0; i < 26; i++) {
			if (steps[i].num_depends == 0 && available > 0) {
				steps[i].num_depends = -1;
				printf("%c ", i + 0x41);

				enqueue(queue, 61 + i, i);
				printf("%d\n", available);
				available--;

				i = 0;
			}
		}

		t++;

		for (int i = 0; i < 5; i++) {
			if (queue[i].timer > 0) {
				queue[i].timer--;
				if (queue[i].timer == 0) {
					printf("__finished timer %d\n", t);
				}
			}
		}

		for (int i = 0; i < 5; i++) {
			if (queue[i].timer == 0) {
				for (int j = 0; j < 26; j++) {
					if (steps[j].depends[queue[i].to_remove]) {
						printf("removed %c\n", queue[i].to_remove + 0x41);
						steps[j].depends[queue[i].to_remove] = 0;
						steps[j].num_depends--;
						printf("d (%c): %d\n", j + 0x41, steps[j].num_depends);
					}
				}
				dequeue(queue, i);
				available++;
				completed++;
			}
		}
	}

	time_end = clock();

    printf(GREEN "\ndone.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "order of steps" WHITE "\t: " CYAN "VAL\n" RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "time to build" WHITE "\t: " CYAN "%d seconds\n" RESET, t);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
