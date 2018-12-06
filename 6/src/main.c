#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define INPUT_LENGTH 50

#define URC_ROW 1
#define URC_COL 2

typedef struct location {
	uint16_t x;
	uint16_t y;
	uint32_t area;
} location_t;

typedef struct unique_rc {
	uint16_t max_x, min_x;
	uint16_t max_y, min_y;
	uint16_t rows[INPUT_LENGTH];
	uint16_t cols[INPUT_LENGTH];
} unique_rc_t;

void _urc_add(unique_rc_t* urc, uint8_t type, uint16_t value) {
	if (urc) {
		if (type == URC_ROW) {
			int i;
			for (i = 0; i < INPUT_LENGTH; i++) {
				if (urc->rows[i] == value) { return; }
				if (!urc->rows[i]) { break; }
			}

			if (value > urc->max_y) { urc->max_y = value; }
			if (value < urc->min_y || i == 0) { urc->min_y = value; }
			urc->rows[i] = value;
		} else {
			int i;
			for (i = 0; i < INPUT_LENGTH; i++) {
				if (urc->cols[i] == value) { return; }
				if (!urc->cols[i]) { break; }
			}

			if (value > urc->max_x) { urc->max_x = value; }
			if (value < urc->min_x || i == 0) { urc->min_x = value; }
			urc->cols[i] = value;
		}
	}
}

void urc_add_row(unique_rc_t* urc, uint16_t y) {
	_urc_add(urc, URC_ROW, y);
}

void urc_add_col(unique_rc_t* urc, uint16_t x) {
	_urc_add(urc, URC_COL, x);
}

uint8_t _urc_has(unique_rc_t* urc, uint8_t type, uint16_t value) {
	if (urc) {
		if (type == URC_ROW) {
			for (int i = 0; i < INPUT_LENGTH; i++) {
				if (urc->rows[i] == value) { return 1; }
				if (!urc->rows[i]) { return 0; }
			}
		} else {
			for (int i = 0; i < INPUT_LENGTH; i++) {
				if (urc->cols[i] == value) { return 1; }
				if (!urc->cols[i]) { return 0; }
			}
		}
	}
	return 0;
}

uint8_t urc_has_y(unique_rc_t* urc, uint16_t value) {
	return _urc_has(urc, URC_ROW, value);
}

uint8_t urc_has_x(unique_rc_t* urc, uint16_t value) {
	return _urc_has(urc, URC_COL, value);
}

uint8_t has_inf_area(unique_rc_t* urc, location_t* coord) {
	return (coord->x >= urc->max_x || coord->x <= urc->min_x || coord->y >= urc->max_y || coord->y <= urc->min_y);
}

uint8_t is_coord(location_t** locations, uint16_t x, uint16_t y) {
	for (int i = 0; i < INPUT_LENGTH; i++) {
		if (locations[i]->x == x && locations[i]->y == y) { return 1; }
	}
	return 0;
}

uint16_t distance_to(location_t* loc, uint16_t x, uint16_t y) {
	uint16_t dist_x, dist_y;
	if (loc->x > x) {
		dist_x = loc->x - x;
	} else {
		dist_x = x - loc->x;
	}
	if (loc->y > y) {
		dist_y = loc->y - y;
	} else {
		dist_y = y - loc->y;
	}
	return dist_x + dist_y; // abs((int32_t) loc->x - x) + abs((int32_t) loc->y - y);
}

void compute_area(location_t** locations, uint16_t* dists) {
	uint16_t min = 0;
	int8_t min_idx = 0;
	uint8_t num_at_mins = 0;

	for (int8_t i = 0; i < INPUT_LENGTH; i++) {
		if (dists[i] < min || i == 0) {
			min = dists[i];
			min_idx = i;
		}
	}

	for (int8_t i = 0; i < INPUT_LENGTH; i++) {
		if (dists[i] == min) {
			num_at_mins++;
		}
	}

	if (num_at_mins == 1 && dists[min_idx] != 0) {
		// printf(RED "%lx: %d\n" RESET, (size_t) &locations[min_idx]->area, locations[min_idx]->area);
		locations[min_idx]->area++;
		// printf(BLUE "%lx: %d\n" RESET, (size_t) &locations[min_idx]->area, locations[min_idx]->area);
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 6" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// open file
    fp = fopen("./input.txt.real", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end;

	// string temp vars
	char x_str[4];
	char y_str[4];

	uint8_t state = 0;
	uint8_t idx = 0;
	uint8_t i = 0;

	location_t* locations[INPUT_LENGTH];
	unique_rc_t* unique_rc = calloc(sizeof(unique_rc_t), 1);

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		for (int i = 0; i < len; i++) {
			switch (state) {
				case 0:
					if (line[i] == ',') {
						state++;
						x_str[idx] = '\0';
						idx = 0;
					} else { x_str[idx++] = line[i]; }
					break;
				case 1:
					if (line[i] == ' ') { state++; }
					break;
				case 2:
					if (line[i] == '\n') {
						state++;
						y_str[idx] = '\0';
						idx = 0;
					} else { y_str[idx++] = line[i]; }
					break;
			}
		}

		location_t* loc = malloc(sizeof(location_t));
		loc->x = atoi(x_str);
		loc->y = atoi(y_str);
		loc->area = 1;
		locations[i++] = loc;

		urc_add_col(unique_rc, loc->x);
		urc_add_row(unique_rc, loc->y);

		printf("%d, %d\n", loc->x, loc->y);
	}

	uint16_t map_width = (2000 + 1);
	uint16_t map_height = (2000 + 1);
	uint16_t*** map = malloc(sizeof(uint16_t**) * map_width);

	for (int i = 0; i < map_width; i++) {
		map[i] = malloc(sizeof(uint16_t*) * map_height);
		for (int j = 0; j < map_height; j++) {
			map[i][j] = malloc(sizeof(uint16_t) * (INPUT_LENGTH));
		}
	}

	for (int i = 0; i < map_width; i++) {
		for (int j = 0; j < map_height; j++) {
			for (int k = 0; k < INPUT_LENGTH; k++) {
				if (i == unique_rc->max_x || i == unique_rc->min_x || j == unique_rc->max_y && j == unique_rc->min_y) {
					map[i][j][k] = -1;

				} else {
				map[i][j][k] = distance_to(locations[k], i, j);
				}
				// printf("%lx: %d\n", (size_t) &map[i][j][k], map[i][j][k]);
			}
		}
	}

	for (int i = 0; i < map_width; i++) {
		for (int j = 0; j < map_height; j++) {
			if (i < unique_rc->max_x && i > unique_rc->min_x && j < unique_rc->max_y && j > unique_rc->min_y) {
				compute_area(locations, map[i][j]);
			}
		}
	}

	uint32_t max_area = 0;
	for (int i = 0; i < INPUT_LENGTH; i++) {
		printf("area: %d\n", locations[i]->area);
		if (locations[i]->area > max_area) {
			max_area = locations[i]->area;
		}
	}

	printf("max area: %d\n", max_area);


	// // get a set of locations without infinite area
	// for (int i = 0; i < 50; i++) {
	// 	if (has_inf_area(unique_rc, locations[i])) {
	// 		printf("coord %d has infinite area\n", i);
	// 	} else {
	// 		// uint32_t area = find_area(locations, locations[i], locations[i]->x, locations[i]->y);
	// 	}
	// }

	printf("%d_%d, %d_%d\n", unique_rc->min_x, unique_rc->max_x, unique_rc->min_y, unique_rc->max_y);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);

	// free up memory

	for (int i = 0; i < (unique_rc->min_x - unique_rc->max_x); i++) {
		for (int j = 0; j < (unique_rc->min_y - unique_rc->max_y); j++) {
			free(map[i][j]);
		}
		free(map[i]);
	}
	free(map);

	if (line) { free(line); }
	fclose(fp);

	return 0;
}
