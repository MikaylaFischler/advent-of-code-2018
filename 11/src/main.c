#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <time.h>

#include "main.h"

typedef struct thread_data {
	uint16_t min;
	uint16_t max;
	uint16_t x;
	uint16_t y;
	uint16_t size;
	int32_t power;
	int32_t** grid;
} thread_data_t;

/**
 * @brief compute the power at a coordinate
 * @param x the x coord
 * @param y the y coord
 * @param grid_sn the grid serial number
 * @return the power value
 */
int32_t compute_power(uint16_t x, uint16_t y, uint16_t grid_sn) {
	uint16_t rack_id = x + 10;
	int32_t power_level = rack_id * y;
	power_level += grid_sn;
	power_level *= rack_id;
	power_level = (power_level / 100) % 10;
	power_level -= 5;
	return power_level;
}

/**
 * @brief copy a grid
 * @param src the source grid
 * @param dest the destination grid
 */
void duplicate_grid(int32_t** src, int32_t** dest) {
	for (uint16_t x = 0; x < 300; x++) {
		for (uint16_t y = 0; y < 300; y++) {
			dest[x][y] = src[x][y];
		}
	}
}

/**
 * @brief compute maximum value in range
 * @param input the input struct (should be thread_data_t*)
 * @return return data (NULL)
 */
void* compute_max_in_range(void* input) {
	thread_data_t* data = (thread_data_t*) input;
	data->power = data->x = data->y = data->size = 0;

	for (uint16_t s = data->min; s <= data->max; s++) {
		for (uint16_t x = 0; x <= 300 - s; x++) {
			for (uint16_t y = 0; y <= 300 - s; y++) {
				int32_t power_total = 0;
				for (uint16_t _x = x; _x < x + s; _x++) {
					for (uint16_t _y = y; _y < y + s; _y++) {
						power_total += data->grid[_x][_y];
					}
				}

				if (power_total > data->power) {
					data->power = power_total;
					data->x = x + 1;
					data->y = y + 1;
					data->size = s;
				}
			}
		}
	}

	return NULL;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 11" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// timing
	clock_t time_start, time_end;

	printf(YELLOW "starting...\n" RESET);

	struct timespec start, finish;
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);

	time_start = clock();

	uint16_t grid_sn = 3613;
	int32_t grid[300][300];

	// compute power
	for (uint16_t x = 0; x < 300; x++) {
		for (uint16_t y = 0; y < 300; y++) {
			grid[x][y] = compute_power(x + 1, y + 1, grid_sn);
		}
	}

	// part 1
	int16_t p1_max_power = 0;
	uint16_t p1_max_x = 0;
	uint16_t p1_max_y = 0;

	for (uint16_t x = 0; x < 298; x++) {
		for (uint16_t y = 0; y < 298; y++) {
			int16_t power_total = 0;
			for (uint16_t _x = x; _x < x + 3; _x++) {
				for (uint16_t _y = y; _y < y + 3; _y++) {
					power_total += grid[_x][_y];
				}
			}

			if (power_total > p1_max_power) {
				p1_max_power = power_total;
				p1_max_x = x + 1;
				p1_max_y = y + 1;
			}
		}
	}

	// create clone of grid in heap
	int32_t** grid_1 = malloc(sizeof(int32_t*) * 300);
	for (int i = 0; i < 300; i++) { grid_1[i] = malloc(sizeof(int32_t) * 300); }
	for (uint16_t x = 0; x < 300; x++) {
		for (uint16_t y = 0; y < 300; y++) { grid_1[x][y] = grid[x][y]; }
	}

	// build up threads and thread data
	thread_data_t thread_data[6];
	pthread_t threads[6];

	thread_data[0].min = 1;
	thread_data[0].max = 100;
	thread_data[0].grid = grid_1;

	thread_data[1].min = 101;
	thread_data[1].max = 135;
	thread_data[1].grid = grid_1;

	thread_data[2].min = 136;
	thread_data[2].max = 150;
	thread_data[2].grid = grid_1;

	thread_data[3].min = 151;
	thread_data[3].max = 165;
	thread_data[3].grid = grid_1;

	thread_data[4].min = 166;
	thread_data[4].max = 200;
	thread_data[4].grid = grid_1;

	thread_data[5].min = 201;
	thread_data[5].max = 300;
	thread_data[5].grid = grid_1;

	// spawn threads
	pthread_create(&threads[0], NULL, compute_max_in_range, (void*) &thread_data[0]);
	pthread_create(&threads[1], NULL, compute_max_in_range, (void*) &thread_data[1]);
	pthread_create(&threads[2], NULL, compute_max_in_range, (void*) &thread_data[2]);
	pthread_create(&threads[3], NULL, compute_max_in_range, (void*) &thread_data[3]);
	pthread_create(&threads[4], NULL, compute_max_in_range, (void*) &thread_data[4]);
	pthread_create(&threads[5], NULL, compute_max_in_range, (void*) &thread_data[5]);

	// wait for threads
	pthread_join(threads[0], NULL);
	printf("thread 1 joined\n");
	pthread_join(threads[5], NULL);
	printf("thread 6 joined\n");
	pthread_join(threads[1], NULL);
	printf("thread 2 joined\n");
	pthread_join(threads[4], NULL);
	printf("thread 5 joined\n");
	pthread_join(threads[2], NULL);
	printf("thread 3 joined\n");
	pthread_join(threads[3], NULL);
	printf("thread 4 joined\n");

	// determine solution
	int32_t max_power = 0;
	uint16_t max_x = 0;
	uint16_t max_y = 0;
	uint16_t best_size = 0;

	for (int i = 0; i < 6; i++) {
		if (thread_data[0].power > max_power) {
			max_power = thread_data[0].power;
			max_x = thread_data[0].x;
			max_y = thread_data[0].y;
			best_size = thread_data[0].size;
		}
	}

	time_end = clock();

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds (cpu: " RED "%f" WHITE " seconds)\n" RESET, elapsed, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "x,y: power" WHITE "\t\t: " CYAN "%d,%d: power level is %d\n" RESET, p1_max_x, p1_max_y, p1_max_power);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "x,y,size: power" WHITE "\t: " CYAN "%d,%d,%d: power level is %d\n" RESET, max_x, max_y, best_size, max_power);

	// free up memory
	for (int i = 0; i < 300; i++) { free(grid_1[i]); }
	free(grid_1);
	return 0;
}
