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

/**
 * @brief determine order of steps (part 1 solution)
 * @param steps the array of steps (length 26)
 * @param output a string with space for 26 characters and a null terminator
 */
void part_1(step_t* steps, char* output) {
	uint8_t completed = 0;
	uint8_t to_remove = 0;
	uint8_t out_idx = 0;

	while (completed != 26) {
		for (int i = 0; i < 26; i++) {
			if (steps[i].num_depends == 0) {
				steps[i].num_depends = -1;
				to_remove = i;
				completed++;
				output[out_idx++] = i + 0x41;
				break;
			}
		}

		for (int j = 0; j < 26; j++) {
			if (steps[j].depends[to_remove]) {
				steps[j].depends[to_remove] = 0;
				steps[j].num_depends--;
			}
		}
	}

	output[out_idx] = '\0';
}

/**
 * @brief initialize queue entries to proper initial values
 * @param queue the queue to initialize
 */
void init_queue(queued_step_t* queue) {
	for (int i = 0; i < 5; i++) {
		queue[i].done = 1;
		queue[i].timer = -1;
		queue[i].to_remove = 0;
	}
}

/**
 * @brief enqueue a task
 * @param queue the queue to add to
 * @param time the time the task will take
 * @param to_remove the task to remove when complete
 */
void enqueue(queued_step_t* queue, uint8_t time, uint8_t to_remove) {
	for (int i = 0; i < 5; i++) {
		if (queue[i].done) {
			queue[i].done = 0;
			queue[i].timer = time;
			queue[i].to_remove = to_remove;
			break;
		}
	}
}

/**
 * @brief dequeue an element (mark it as done and give it a negative time value)
 * @param queue the queue to dequeue from
 * @param i the element in the queue to dequeue
 */
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

	// timing
	clock_t time_start, time_end;

	// char input temp vars
	char dependency_in = 0x0;
	char step_in = 0x0;
	uint8_t state = 0;

	// step arrays
	step_t* steps_p1 = calloc(sizeof(step_t), 26);
	step_t* steps_p2 = calloc(sizeof(step_t), 26);

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

		steps_p1[step_in - 0x41].depends[dependency_in - 0x41] = 1;
		steps_p1[step_in - 0x41].num_depends++;
		steps_p2[step_in - 0x41].depends[dependency_in - 0x41] = 1;
		steps_p2[step_in - 0x41].num_depends++;
	}

	// PART 1
	char p1_str[27];
	part_1(steps_p1, p1_str);

	// PART 2
	queued_step_t* queue = malloc(sizeof(queued_step_t) * 5);
	init_queue(queue);
	uint16_t time_to_build = 0;
	uint8_t completed = 0;
	uint8_t available = 5;

	while (completed != 26) {
		for (int i = 0; i < 26; i++) {
			if (steps_p2[i].num_depends == 0 && available > 0) {
				steps_p2[i].num_depends = -1;
				available--;
				enqueue(queue, 61 + i, i);
				i = 0;
			}
		}

		time_to_build++;

		for (int i = 0; i < 5; i++) {
			if (queue[i].timer > 0) { queue[i].timer--; }
		}

		for (int i = 0; i < 5; i++) {
			if (queue[i].timer == 0) {
				for (int j = 0; j < 26; j++) {
					if (steps_p2[j].depends[queue[i].to_remove]) {
						steps_p2[j].depends[queue[i].to_remove] = 0;
						steps_p2[j].num_depends--;
					}
				}

				dequeue(queue, i);
				available++;
				completed++;
			}
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "order of steps" WHITE "\t: " CYAN "%s\n" RESET, p1_str);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "time to build" WHITE "\t: " CYAN "%d seconds\n" RESET, time_to_build);

	// free up memory
	free(steps_p1);
	free(steps_p2);
	free(queue);
	if (line) { free(line); }
	fclose(fp);

	return 0;
}
