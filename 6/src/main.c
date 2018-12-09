#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define INPUT_LENGTH 50

#define MAP_PROP_ROW 1
#define MAP_PROP_COL 2

typedef struct location {
	uint16_t x;
	uint16_t y;
	uint32_t area;
} location_t;

typedef struct map_prop {
	uint16_t max_x, min_x;
	uint16_t max_y, min_y;
} map_prop_t;

/**
 * @brief update the map properties (max/min values)
 * @param map_prop the map peroperties
 * @param type the type (x or y)
 * @param value the new value to set if is a max or min
 */
void map_prop_update(map_prop_t* map_prop, uint8_t type, uint16_t value) {
	if (map_prop) {
		if (type == MAP_PROP_ROW) {
			if (value > map_prop->max_y) { map_prop->max_y = value; }
			if (value < map_prop->min_y || map_prop->min_y == 0) { map_prop->min_y = value; }
		} else {
			if (value > map_prop->max_x) { map_prop->max_x = value; }
			if (value < map_prop->min_x || map_prop->min_x == 0) { map_prop->min_x = value; }
		}
	}
}

/**
 * @brief check if an x and y are at the coordinate of a location
 * @param locations the array of location structs
 * @param x the x coodinate
 * @param y the y coordinate
 * @return 1 if is a location, 0 otherwise
 */
uint8_t is_coord(location_t** locations, uint16_t x, uint16_t y) {
	for (int i = 0; i < INPUT_LENGTH; i++) {
		if (locations[i]->x == x && locations[i]->y == y) { return 1; }
	}

	return 0;
}

/**
 * @brief check if a coordinate of a location is at the boundaries
 * @param map_prop the map properties
 * @param coord the location to check
 * @return 1 if at boundaries, 0 otherwise
 */
uint8_t at_bounds(map_prop_t* map_prop, location_t* coord) {
	return (coord->x == map_prop->max_x || coord->x == map_prop->min_x || coord->y == map_prop->max_y || coord->y == map_prop->min_y);
}

/**
 * @brief check if a location has an infinite area
 * @param map_prop the map properties
 * @param locations the locations in the map
 * @param coord the location to check
 * @return 1 if has infinite area, 0 otherwise
 */
uint8_t has_inf_area(map_prop_t* map_prop, location_t** locations, location_t* coord) {
	if (at_bounds(map_prop, coord)) { return 1; }

	uint16_t mid_x = (map_prop->max_x + map_prop->min_x) / 2;
	uint16_t mid_y = 0;
	uint16_t crit_distance = 0;
	uint8_t hits = 0;

	if (coord->x > mid_x) {
		for (uint16_t x = coord->x; x < map_prop->max_x; x++) {
			crit_distance = map_prop->max_x - x;
			if (x <= map_prop->max_x && x >= map_prop->min_x) {
				for (uint16_t y = (coord->y - crit_distance); y < (coord->y + crit_distance); y++) {
					if (is_coord(locations, x, y)) { hits++; }
				}
			}
		}
	} else if (coord->x < mid_x) {
		for (uint16_t x = map_prop->min_x; x < coord->x; x++) {
			crit_distance = x - map_prop->min_x;
			if (x <= map_prop->max_x && x >= map_prop->min_x) {
				for (uint16_t y = (coord->y - crit_distance); y < (coord->y + crit_distance); y++) {
					if (is_coord(locations, x, y)) { hits++; }
				}
			}
		}
	} else if (coord->y > (mid_y = (map_prop->max_y + map_prop->min_y) / 2)) {
		for (uint16_t y = coord->y; y < map_prop->max_y; y++) {
			crit_distance = map_prop->max_y - y;
			if (y <= map_prop->max_y && y >= map_prop->min_y) {
				for (uint16_t x = (coord->x - crit_distance); x < (coord->x + crit_distance); x++) {
					if (is_coord(locations, x, y)) { hits++; }
				}
			}
		}
	} else if (coord->y < mid_y) {
		for (uint16_t y = map_prop->min_y; y < coord->y; y++) {
			crit_distance = y - map_prop->min_y;
			if (y <= map_prop->max_y && y >= map_prop->min_y) {
				for (uint16_t x = (coord->x - crit_distance); x < (coord->x + crit_distance); x++) {
					if (is_coord(locations, x, y)) { hits++; }
				}
			}
		}
	}

	if (hits > 0) { return 1; }
	return 0;
}

/**
 * @brief find the distance to a point from a coordinate
 * @param loc the location to check from
 * @param x the x position of the point
 * @param y the y position of the point
 * @return absolute value of the distance
 */
uint16_t distance_to(location_t* loc, int16_t x, int16_t y) {
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

/**
 * @brief check if an array of distances totals less than 10000
 * @param dists the distances
 * @return 1 if is less than 10000, 0 otherwise
 */
uint8_t is_less_than_10000(uint16_t* dists) {
	uint16_t dist = 0;
	for (int8_t i = 0; i < INPUT_LENGTH; i++) { dist += dists[i]; }
	if (dist < 10000) { return 1; }
	return 0;
}

/**
 * @brief update the area of locations
 * @param locations the locations
 * @param dists the distances to check
 */
void compute_area(location_t** locations, uint16_t* dists) {
	uint16_t min = 0;
	uint8_t min_idx = 0;
	uint8_t num_at_mins = 0;

	for (uint8_t i = 0; i < INPUT_LENGTH; i++) {
		if (dists[i] < min || i == 0) {
			min = dists[i];
			min_idx = i;
		}
	}

	for (uint8_t i = 0; i < INPUT_LENGTH; i++) {
		if (dists[i] == min) { num_at_mins++; }
	}

	if (num_at_mins == 1) { locations[min_idx]->area++; }
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
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, parse_end, map_alloc_start, map_alloc_end, distance_start,
		distance_end, compute_start, compute_end;

	// string temp vars
	char x_str[4];
	char y_str[4];

	uint8_t state = 0;
	uint8_t idx = 0;
	uint8_t i = 0;

	location_t* locations[INPUT_LENGTH];
	map_prop_t* map_prop = calloc(sizeof(map_prop_t), 1);

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	printf(YELLOW ">" B_WHITE " parsing input...\n" RESET);

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
		loc->area = 0;
		locations[i++] = loc;

		map_prop_update(map_prop, MAP_PROP_COL, loc->x);
		map_prop_update(map_prop, MAP_PROP_ROW, loc->y);
	}

	parse_end = clock();

	printf(B_GREEN ">" B_WHITE " parsed input " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (parse_end - time_start) * 1000.0 / CLOCKS_PER_SEC);

	printf(YELLOW ">" B_WHITE " allocating map...\n" RESET);

	map_alloc_start = clock();

	uint16_t map_width = (map_prop->max_x - map_prop->min_y + 1);
	uint16_t map_height = (map_prop->max_y - map_prop->min_y + 1);
	uint16_t*** map = malloc(sizeof(uint16_t**) * map_width);
	uint32_t region = 0;
	uint16_t x, y;

	for (int i = 0; i < map_width; i++) {
		map[i] = malloc(sizeof(uint16_t*) * map_height);
		for (int j = 0; j < map_height; j++) {
			map[i][j] = malloc(sizeof(uint16_t) * (INPUT_LENGTH));
		}
	}

	map_alloc_end = clock();

	printf(B_GREEN ">" B_WHITE " allocated map " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (map_alloc_end - map_alloc_start) * 1000.0 / CLOCKS_PER_SEC);

	printf(YELLOW ">" B_WHITE " computing distances to coordinates...\n" RESET);

	distance_start = clock();

	for (int i = 0; i < map_width; i++) {
		for (int j = 0; j < map_height; j++) {
			x = i + map_prop->min_x;
			y = j + map_prop->min_y;

			for (int k = 0; k < INPUT_LENGTH; k++) {
				if (x == map_prop->max_x || x == map_prop->min_x || y == map_prop->max_y && y == map_prop->min_y) {
					map[i][j][k] = -1;
				} else {
					map[i][j][k] = distance_to(locations[k], x  , y );
				}
			}
		}
	}

	distance_end = clock();

	printf(B_GREEN ">" B_WHITE " computed distances to coordinates " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (distance_end - distance_start) * 1000.0 / CLOCKS_PER_SEC);

	printf(YELLOW ">" B_WHITE " computing areas (p1) and largest region (p2)...\n" RESET);

	compute_start = clock();

	for (int i = 0; i < map_width; i++) {
		for (int j = 0; j < map_height; j++) {
			x = i + map_prop->min_x;
			y = j + map_prop->min_y;

			if (x < map_prop->max_x && x > map_prop->min_x && y < map_prop->max_y && y > map_prop->min_y) {
				compute_area(locations, map[i][j]);
				region += is_less_than_10000(map[i][j]);
			}
		}
	}

	uint32_t max_area = 0;
	for (int i = 0; i < INPUT_LENGTH; i++) {
		if (locations[i]->area > max_area && has_inf_area(map_prop, locations, locations[i])) {
			max_area = locations[i]->area;
		}
	}

	compute_end = clock();

	printf(B_GREEN ">" B_WHITE " computed areas, max area, and largest region " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (compute_end - compute_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "largest finite area" WHITE "\t: " CYAN "%d\n" RESET, max_area);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "size of region" WHITE "\t\t: " CYAN "%d\n" RESET, region);

	// free up memory

	for (int i = 0; i < (map_prop->min_x - map_prop->max_x); i++) {
		for (int j = 0; j < (map_prop->min_y - map_prop->max_y); j++) {
			free(map[i][j]);
		}
		free(map[i]);
	}
	free(map);

	if (line) { free(line); }
	fclose(fp);

	return 0;
}
