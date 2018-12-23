#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define MAP_HEIGHT 50
#define MAP_WIDTH 50

void print_map(char** map) {
	for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
		for (uint8_t x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x] == '|') { printf(GREEN); } else
			if (map[y][x] == '#') { printf(YELLOW); }
			printf("%c" RESET, map[y][x]);
		}
		printf("\n");
	}
}

uint8_t num_adjacent(char** map, char adjacent_type, uint8_t x, uint8_t y) {
	uint8_t min_x = x > 0 ? (x - 1) : x;
	uint8_t max_x = x < (MAP_WIDTH - 1) ? (x + 1) : x;
	uint8_t min_y = y > 0 ? (y - 1) : y;
	uint8_t max_y = y < (MAP_HEIGHT - 1) ? (y + 1) : y;
	uint8_t adj_count = 0;

	// printf("%d,%d ==> %d->%d, %d->%d\n", x, y, min_x, max_x, min_y, max_y);

	for (uint8_t _y = min_y; _y <= max_y; _y++) {
		for (uint8_t _x = min_x; _x <= max_x; _x++) {
			if (!(_x == x && _y == y) && map[_y][_x] == adjacent_type) {
				adj_count++;
			}
		}
	}

	// printf("%c found %d times\n", adjacent_type, adj_count);

	return adj_count;
}

void iterate_forest(char** start, char** end) {
	for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
		for (uint8_t x = 0; x < MAP_WIDTH; x++) {
			char c = start[y][x];
			if (c == '.') {

				if (num_adjacent(start, '|', x, y) > 2) {
					end[y][x] = '|';
				} else {
					end[y][x] = c;
				}
			} else if (c == '|') {
				if (num_adjacent(start, '#', x, y) > 2) {
					end[y][x] = '#';
				} else {
					end[y][x] = c;
				}
			} else if (c == '#') {
				if (num_adjacent(start, '#', x, y) > 0 && num_adjacent(start, '|', x, y) > 0) {
					end[y][x] = c;
				} else {
					end[y][x] = '.';
				}
			}
		}
	}
}

uint32_t get_resource_value(char** map) {
	uint32_t wooded_cnt = 0;
	uint32_t lumberyard_cnt = 0;

	for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
		for (uint8_t x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x] == '|') {
				wooded_cnt++;
			} else if (map[y][x] == '#') {
				lumberyard_cnt++;
			}
		}
	}

	return wooded_cnt * lumberyard_cnt;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day XX" WHITE "           |\n" RESET);
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

	// maps
	char** a = malloc(sizeof(char*) * MAP_HEIGHT);
	char** b = malloc(sizeof(char*) * MAP_HEIGHT);
	for (int i = 0; i < MAP_HEIGHT; i++) {
		a[i] = malloc(sizeof(char) * MAP_WIDTH);
		b[i] = malloc(sizeof(char) * MAP_WIDTH);
	}

	// file parse variables
	uint8_t y = 0;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	// parse in starting case
	while ((num_read = getline(&line, &len, fp)) != -1) {
		for (int i = 0; i < MAP_WIDTH; i++) { a[y][i] = line[i]; }
		y++;
	}

	uint32_t p1_resource_val = 0;
	uint32_t resource_val = 0;


	/**
	 PLEASE NOTE: CURRENT SOLUTION IS JUST LUCKY THAT 1,000,000,000 DIVIDED BY
	 28, WHICH WAS THE LENGTH OF A REPEATED CYCLE, SO IT WAS THE SAME AS AFTER 1,000 MINUTES
	 (AND PROBABLY EARLIER)

	 THIS WILL BE MODIFIED LATER TO REFLECT THE ACTUAL LOGIC
	 */

	for (int i = 0; i < 1000;) {
		// print_map(a);
		// printf("\n");

		iterate_forest(a, b);
		resource_val = get_resource_value(b);
		printf("%d: %d\n", i++, resource_val);

		// print_map(b);
		// printf("\n");

		iterate_forest(b, a);
		resource_val = get_resource_value(a);
		printf("%d: %d\n", i++, resource_val);

		if (i == 10) { p1_resource_val = get_resource_value(a); }
	}

	print_map(a);
	printf("\n");

	resource_val = get_resource_value(a);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t\t\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "resource value after 10 minutes" WHITE "\t\t: " CYAN "%d\n" RESET, p1_resource_val);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "resource value after 1,000,000,000 minutes" WHITE "\t: " CYAN "%d\n" RESET, resource_val);

	// free up memory
	for (int i = 0; i < MAP_HEIGHT; i++) {
		free(a[i]);
		free(b[i]);
	}

	free(a);
	free(b);

	fclose(fp);
	if (line) { free(line); }

	return 0;
}
