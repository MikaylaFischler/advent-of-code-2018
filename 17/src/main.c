#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

typedef struct constraint {
	uint16_t min_x;
	uint16_t min_y;
	uint16_t max_x;
	uint16_t max_y;
	uint16_t height;
	uint16_t width;
} constraint_t;

typedef struct point {
	int16_t x, y;
} point_t;

/**
 * @brief print out a colored version of the map
 * @param map the map
 * @param constraints a pointer to the constraints
 */
void print_map(char** map, const constraint_t* constraints) {
	for (uint16_t y = 0; y < constraints->height; y++) {
		for (uint16_t x = 0; x < constraints->width; x++) {
			if (map[y][x] == '#') { printf(B_YELLOW); } else
			if (map[y][x] == '~') { printf(BLUE); } else
			if (map[y][x] == '|') { printf(CYAN); }
			printf("%c" RESET, map[y][x]);
		}
		printf("\n");
	}
}

/**
 * @brief count all water
 * @param map the map
 * @param constraints a pointer to the constraints
 */
uint32_t count_water(char** map, const constraint_t* constraints) {
	uint32_t count = 0;
	for (uint16_t y = 0; y < constraints->height; y++) {
		for (uint16_t x = 0; x < constraints->width; x++) {
			if (map[y][x] == '~' || map[y][x] == '|') { count++; }
		}
	}
	return count;
}

/**
 * @brief count only standing water
 * @param map the map
 * @param constraints a pointer to the constraints
 */
uint32_t count_standing_water(char** map, const constraint_t* constraints) {
	uint32_t count = 0;
	for (uint16_t y = 0; y < constraints->height; y++) {
		for (uint16_t x = 0; x < constraints->width; x++) {
			if (map[y][x] == '~') { count++; }
		}
	}
	return count;
}

/**
 * @brief insert clay into a map
 * @param map the map
 * @param constraints a pointer to the constraints
 * @param c1_type the constant coordinate (t x or y)
 * @param c1 the constant coordinate
 * @param r1 the start of the ranged coordinates
 * @param r2 the end of the ranged coordinates
 */
void insert_clay(char** map, const constraint_t* constraints, char c1_type, uint16_t c1, uint16_t r1, uint16_t r2) {
	if (c1_type == 'x') {
		c1 -= constraints->min_x;
		r1 -= constraints->min_y;
		r2 -= constraints->min_y;
		for (int i = r1; i <= r2; i++) { map[i][c1] = '#'; }
	} else {
		c1 -= constraints->min_y;
		r1 -= constraints->min_x;
		r2 -= constraints->min_x;
		//! @brief has cache speed advantage
		for (int i = r1; i <= r2; i++) { map[c1][i] = '#'; }
	}
}

/**
 * @brief fill a region with water
 * @param map the map
 * @param constraints a pointer to the constraints
 * @param point the point to start filling from
 * @return an array of 2 points, for where overflows occured.<br/>
 *          if one or both did not occur, that point's x and y positions will be -1
 */
point_t* fill_region(char** map, const constraint_t* constraints, const point_t* point) {
	uint16_t x = point->x;
	uint16_t y = point->y;
	int16_t x1 = x;
	int16_t x2 = x + 1;
	uint8_t overflowed = 0;

	while (1) {
		while (x1 > 0 && map[y][x1] != '#' && map[y+1][x1] != '.') { map[y][x1--] = '~'; }
		while (x2 < constraints->width - 1 && map[y][x2] != '#' && map[y+1][x2] != '.') { map[y][x2++] = '~'; }

		overflowed = map[y+1][x1] == '.' || map[y+1][x2] == '.';
		if (overflowed) {
			x1 = x;
			x2 = x + 1;
			while (x1 > 0 && map[y][x1] == '~' && map[y+1][x1] != '.') { map[y][x1--] = '|'; }
			while (x2 < constraints->width - 1 && map[y][x2] == '~' && map[y+1][x2] != '.') { map[y][x2++] = '|'; }
			break;
		}

		x1 = x;
		x2 = x + 1;
		y--;
	}

	point_t* points = malloc(sizeof(point_t) * 2);
	points[0].x = -1;
	points[0].y = -1;
	points[1].x = -1;
	points[1].y = -1;

	// overflow it by 1

	if (map[y][x1] == '.') {
		map[y][x1] = '|';
		if (map[y+1][x1] == '.') {
			points[0].x = x1;
			points[0].y = y;
		}
	}

	if (map[y][x2] == '.') {
		map[y][x2] = '|';
		if (map[y+1][x2] == '.') {
			points[1].x = x2;
			points[1].y = y;
		}
	}

	return points;
}

/**
 * @brief propagate a line of water down (and recursively complete propagation)
 * @param map the map
 * @param constraints a pointer to the constraints
 * @param point the point to start propagation
 */
void propagate_water(char** map, const constraint_t* constraints, const point_t* point) {
	uint16_t x = point->x;
	uint16_t y = point->y;
	uint8_t no_clay = 1;

	if (map[y][x] != '.' && map[y][x] != '|') { return; }

	while (no_clay && y < constraints->height) {
		if (map[y][x] == '|') { return; }
		if (map[y][x] != '.') { break; }
		map[y++][x] = '|';
	}

	if (y == constraints->height) { return; }
	// print_map(map, constraints);

	point_t p = { .x = x, .y = --y };
	point_t* points = fill_region(map, constraints, &p);
	// print_map(map, constraints);

	for (int i = 0; i < 2; i++) {
		if (points[i].x != -1) {
			point_t _p = { .x = points[i].x, .y = points[i].y + 1 };
			propagate_water(map, constraints, &_p);
			// print_map(map, constraints);
		}
	}

	free(points);
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 17" WHITE "           |\n" RESET);
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

	// file parsing variables
	char c1_type;
	char c1_str[5];	// coordinate component 1
	char r1_str[5];	// range start
	char r2_str[5];	// range end
	uint8_t i;
	uint8_t offset;

	uint16_t c1, r1, r2;

	// map constraints
	constraint_t constraints = { .min_x = -1, .min_y = -1, .max_x = 0, .max_y = 0 };

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		c1_type = line[0];

		for (i = 0; i < 5 && line[i + 2] != ','; i++) {
			c1_str[i] = line[i + 2];
		}
		c1_str[i] = '\0';
		offset = i + 6;

		for (i = 0; i < 5 && line[i + offset] != '.'; i++) {
			r1_str[i] = line[i + offset];
		}
		r1_str[i] = '\0';

		offset = i + offset + 2;

		for (i = 0; i < 5 && line[i + offset] != '\n'; i++) {
			r2_str[i] = line[i + offset];
		}
		r2_str[i] = '\0';

		c1 = atoi(c1_str);
		r1 = atoi(r1_str);
		r2 = atoi(r2_str);

		if (c1_type == 'x') {
			if (constraints.min_x > c1) { constraints.min_x = c1; }
			else if (constraints.max_x < c1) { constraints.max_x = c1; }

			if (constraints.min_y > r1) { constraints.min_y = r1; }
			else if (constraints.max_y < r1) { constraints.max_y = r1; }

			if (constraints.min_y > r2) { constraints.min_y = r2; }
			else if (constraints.max_y < r2) { constraints.max_y = r2; }
		} else {
			if (constraints.min_y > c1) { constraints.min_y = c1; }
			else if (constraints.max_y < c1) { constraints.max_y = c1; }

			if (constraints.min_x > r1) { constraints.min_x = r1; }
			else if (constraints.max_x < r1) { constraints.max_x = r1; }

			if (constraints.min_x > r2) { constraints.min_x = r2; }
			else if (constraints.max_x < r2) { constraints.max_x = r2; }
		}
	}

	printf(GREEN "=> " B_WHITE "found bounds: " WHITE "min_x=%d, min_y=%d, max_x=%d, max_y=%d\n" RESET, constraints.min_x, constraints.min_y, constraints.max_x, constraints.max_y);

	// determine size with padding (range is diff+1, so add 2 for before and after for x but only before for y)
	constraints.height = constraints.max_y - constraints.min_y + 2;
	constraints.width = constraints.max_x - constraints.min_x + 3;
	char** map = malloc(sizeof(char*) * constraints.height);
	for (int i = 0; i < constraints.height; i++) {
		map[i] = malloc(sizeof(char) * constraints.width);
		for (int x = 0; x < constraints.width; x++) { map[i][x] = '.'; }
	}

	// enforce padding
	constraints.min_x--;
	constraints.min_y--;

	printf(GREEN "=> " B_WHITE "allocated map\n" RESET);

	rewind(fp);

	printf(BLUE "=> " B_WHITE "reset file\n" RESET);

	printf(YELLOW "=> " B_WHITE "inserting clay...\n" RESET);
	clock_t clay_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		c1_type = line[0];

		for (i = 0; i < 5 && line[i + 2] != ','; i++) {
			c1_str[i] = line[i + 2];
		}
		c1_str[i] = '\0';
		offset = i + 6;

		for (i = 0; i < 5 && line[i + offset] != '.'; i++) {
			r1_str[i] = line[i + offset];
		}
		r1_str[i] = '\0';

		offset = i + offset + 2;

		for (i = 0; i < 5 && line[i + offset] != '\n'; i++) {
			r2_str[i] = line[i + offset];
		}
		r2_str[i] = '\0';

		c1 = atoi(c1_str);
		r1 = atoi(r1_str);
		r2 = atoi(r2_str);

		insert_clay(map, &constraints, c1_type, c1, r1, r2);
	}

	printf(GREEN "=> " B_WHITE "inserted clay " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - clay_start) * 1000.0 / CLOCKS_PER_SEC);

	// print_map(map, &constraints);

	point_t point = { .x = 500 - constraints.min_x, .y = 1 };

	printf(YELLOW "=> " B_WHITE "propagating water...\n" RESET);

	clock_t prop_start = clock();
	propagate_water(map, &constraints, &point);

	printf(GREEN "=> " B_WHITE "propagated water " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - prop_start) * 1000.0 / CLOCKS_PER_SEC);

	// print_map(map, &constraints);

	uint32_t num_water = count_water(map, &constraints);
	uint32_t num_standing_water = count_standing_water(map, &constraints);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "count of water" WHITE "\t\t\t: " CYAN "%d\n" RESET, num_water);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "count of standing water" WHITE "\t: " CYAN "%d\n" RESET, num_standing_water);

	// free up memory
	for (uint16_t i = 0; i < constraints.height; i++) { free(map[i]); }
	free(map);
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
