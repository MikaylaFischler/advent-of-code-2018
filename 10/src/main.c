#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>

#include "main.h"

#define NUM_POINTS 334

typedef struct point {
	int64_t x;
	int64_t y;
	int8_t v_x;
	int8_t v_y;
} point_t;

/**
 * @brief get the density factor (the width + the height)
 * @param points the points array of size NUM_POINTS
 * @return the density factor
 */
uint64_t get_density_factor(point_t* points) {
	int64_t max_x = 0;
	int64_t max_y = 0;
	int64_t min_x = 0;
	int64_t min_y = 0;

	for (int i = 0; i < NUM_POINTS; i++) {
		if (i == 0) {
			max_x = min_x = points[i].x;
			max_y = min_y = points[i].y;
		} else {
			if (points[i].x > max_x) {
				max_x = points[i].x;
			} else if (points[i].x < min_x) {
				min_x = points[i].x;
			}

			if (points[i].y > max_y) {
				max_y = points[i].y;
			} else if (points[i].y < min_y) {
				min_y = points[i].y;
			}
		}
	}

	return abs(max_x - min_x) + abs(max_y - min_y);
}

/**
 * @brief move the points by adding their velocity to their position
 * @param points the points array of size NUM_POINTS
 */
void move_points(point_t* points) {
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i].x += points[i].v_x;
		points[i].y += points[i].v_y;
	}
}


/**
 * @brief undo a move by subtracting their velocity from their position
 * @param points the points array of size NUM_POINTS
 */
void undo_move_points(point_t* points) {
	for (int i = 0; i < NUM_POINTS; i++) {
		points[i].x -= points[i].v_x;
		points[i].y -= points[i].v_y;
	}
}


/**
 * @brief print out the points
 * @param points the points array of size NUM_POINTS
 */
void print_sky(point_t* points) {
	int64_t max_x = 0;
	int64_t max_y = 0;
	int64_t min_x = 0;
	int64_t min_y = 0;

	for (int i = 0; i < NUM_POINTS; i++) {
		if (i == 0) {
			max_x = min_x = points[i].x;
			max_y = min_y = points[i].y;
		} else {
			if (points[i].x > max_x) {
				max_x = points[i].x;
			} else if (points[i].x < min_x) {
				min_x = points[i].x;
			}

			if (points[i].y > max_y) {
				max_y = points[i].y;
			} else if (points[i].y < min_y) {
				min_y = points[i].y;
			}
		}
	}

	uint8_t points_map[max_x - min_x + 1][max_y - min_y + 1];

	for (int x = 0; x <= max_x - min_x; x++) {
		for (int y = 0; y <= max_y - min_y; y++) {
			points_map[x][y] = 0;
			for (int i = 0; i < NUM_POINTS; i++) {
				if ((x + min_x) == points[i].x && (y + min_y) == points[i].y) {
					points_map[x][y] = 1;
					break;
				}
			}
		}
	}

	for (int y = 0; y <= max_y - min_y; y++) {
		for (int x = 0; x <= max_x - min_x; x++) {
			if (points_map[x][y]) {
				printf("%c", '#');
			} else {
				printf("%c", ' ');
			}
		}
		printf("\n");
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 10" WHITE "           |\n" RESET);
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
	clock_t time_start, time_end, iterate_start;

	// temp strings
	char x_str[7];
	char y_str[7];
	char v_x_str[3];
	char v_y_str[3];

	uint8_t state = 0;
	uint8_t idx = 0;

	point_t points[NUM_POINTS];
	uint16_t point_idx = 0;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		for (int i = 0; i < len; i++) {
			switch (state) {
				case 0:
					if (line[i] == '<') { state++; }
					break;
				case 1:
					if (line[i] == ',') {
						state++;
						x_str[idx] = '\0';
						idx = 0;
					} else { x_str[idx++] = line[i]; }
					break;
				case 2:
					if (line[i] == ' ') { state++; }
					break;
				case 3:
					if (line[i] == '>') {
						state++;
						y_str[idx] = '\0';
						idx = 0;
					} else { y_str[idx++] = line[i]; }
					break;
				case 4:
					if (line[i] == '<') { state++; }
					break;
				case 5:
					if (line[i] == ',') {
						state++;
						v_x_str[idx] = '\0';
						idx = 0;
					} else { v_x_str[idx++] = line[i]; }
					break;
				case 6:
					if (line[i] == ' ') { state++; }
					break;
				case 7:
					if (line[i] == '>') {
						state++;
						v_y_str[idx] = '\0';
						idx = 0;
					} else { v_y_str[idx++] = line[i]; }
					break;
			}
		}

		if (state == 8) {
			points[point_idx].x = atoi(x_str);
			points[point_idx].y = atoi(y_str);
			points[point_idx].v_x = atoi(v_x_str);
			points[point_idx].v_y = atoi(v_y_str);
			point_idx++;
		}
	}

	printf(B_GREEN ">" B_WHITE " finished creating points from input file " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - time_start) * 1000.0 / CLOCKS_PER_SEC);

	iterate_start = clock();

	uint64_t density = 0;
	uint32_t seconds = 0;

	uint64_t min_density_factor = -1;
	uint32_t min_density_time = 0;

	density = get_density_factor(points);

	while (1) {
		if (density < min_density_factor) {
			min_density_factor = density;
			min_density_time = seconds;
		}

		if (density > min_density_factor) { break; }

		move_points(points);
		density = get_density_factor(points);
		seconds++;
	}

	printf(B_GREEN ">" B_WHITE " found most dense state " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - iterate_start) * 1000.0 / CLOCKS_PER_SEC);
	printf(B_YELLOW ">" B_WHITE " undoing last move...\n" RESET);

	undo_move_points(points);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "answer" WHITE "\t\t: " CYAN "\n");
	print_sky(points);
	printf(RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "time to answer" WHITE "\t: " CYAN "%d seconds\n" RESET, min_density_time);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
