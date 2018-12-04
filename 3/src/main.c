#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define FABRIC_SIZE 1000
#define NUM_ENTRIES 1287

typedef struct entry {
	uint16_t claim;
	uint16_t top, left, width, height;
} entry_t;

/**
 * @brief mark a region of fabric as claimed
 * @param fabric the fabric array
 * @param e the entry to use to claim
 */
void claim_fabric(uint8_t** fabric, const entry_t e) {
	for (int x = e.left; x < (e.left + e.width); x++) {
		for (int y = e.top; y < (e.top + e.height); y++) {
			if (fabric[x][y] < 2) { fabric[x][y]++; }
		}
	}
}

/**
 * @brief check if the provided entry overlaps with any parts of the fabric
 * @param fabric the fabric array
 * @param e the entry to check
 * @return 1 if overlaps, 0 otherwise
 */
uint8_t overlaps(uint8_t** fabric, const entry_t e) {
	for (int x = e.left; x < (e.left + e.width); x++) {
		for (int y = e.top; y < (e.top + e.height); y++) {
			if (fabric[x][y] > 1) { return 1; }
		}
	}
	return 0;
}

/**
 * @brief count how many inches have been claimed by more than 1 claim
 * @param fabric the fabric array
 * @return count of inches claimed more than once
 */
uint32_t count_multi_claimed(uint8_t** fabric) {
	uint32_t count = 0;
	for (int x = 0; x < FABRIC_SIZE; x++) {
		for (int y = 0; y < FABRIC_SIZE; y++) {
			if (fabric[x][y] > 1) { count++; }
		}
	}
	return count;
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 3" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

    clock_t time_start, time_end, claim_start, claim_end, claim_fabric_total_start,
		claim_fabric_total_end, multi_count_start, multi_count_end, overlap_start, overlap_end;

	// allocate fabric array (1MB)
	uint8_t** fabric = calloc(sizeof(uint8_t*), FABRIC_SIZE);
	for (int x = 0; x < FABRIC_SIZE; x++) {
		fabric[x] = calloc(sizeof(uint8_t), FABRIC_SIZE);
	}

	// entry table
	entry_t entries[NUM_ENTRIES];

	// temp variables
	char claim_str[5];
	char top_str[5];
	char left_str[5];
	char width_str[5];
	char height_str[5];

	// counters and states
	uint32_t count = 0;
	uint16_t entry_idx = 0;
	uint16_t non_overlap_claim = 0;
	uint8_t state = 0;
	uint8_t x = 0;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// check verbosity
	uint8_t verbose = !(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'q');

	printf(WHITE "verbosity set to " CYAN "%d\n" RESET, verbose);
    printf(YELLOW "starting...\n" RESET);

	claim_fabric_total_start = time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		if (verbose) { claim_start = clock(); }
		for (int i = 0; i < len; i++) {
			switch (state) {
				case 0:
					if (line[i] == '#') { state++; }
					break;
				case 1:
					if (line[i] == ' ') {
						state++;
						claim_str[x] = '\0';
						x = 0;
					} else { claim_str[x++] = line[i]; }
					break;
				case 2:
					if (line[i] == ' ') { state++; }
					break;
				case 3:
					if (line[i] == ',') {
						state++;
						left_str[x] = '\0';
						x = 0;
					} else { left_str[x++] = line[i]; }
					break;
				case 4:
					if (line[i] == ':') {
						state++;
						top_str[x] = '\0';
						x = 0;
					} else { top_str[x++] = line[i]; }
					break;
				case 5:
					if (line[i] == ' ') { state++; }
					break;
				case 6:
					if (line[i] == 'x') {
						state++;
						width_str[x] = '\0';
						x = 0;
					} else { width_str[x++] = line[i]; }
					break;
				case 7:
					if (line[i] == '\n') {
						state++;
						height_str[x] = '\0';
						x = 0;
					} else { height_str[x++] = line[i]; }
					break;
			}
		}

		if (state == 8) {
			entry_t* e = &entries[entry_idx++];
			e->claim = atoi(claim_str);
			e->left = atoi(left_str);
			e->top = atoi(top_str);
			e->width = atoi(width_str);
			e->height = atoi(height_str);

			claim_fabric(fabric, *e);

			if (verbose) {
				claim_end = clock();
				printf(B_YELLOW ">" WHITE " claimed fabric for entry " B_WHITE "%d" WHITE " (%d,%d) with width %d and height %d (" BLUE "%ld us" WHITE ")\n",
					e->claim, e->left, e->top, e->width, e->height, (claim_end - claim_start) * 1000000 / CLOCKS_PER_SEC);
			}
		}
	}
	claim_fabric_total_end = clock();

	printf(B_GREEN ">" B_WHITE " finished fabric claims " WHITE "(" BLUE "%f seconds" WHITE ")\n" RESET, (double) (claim_fabric_total_end - claim_fabric_total_start) / CLOCKS_PER_SEC);

	multi_count_start = clock();
	count = count_multi_claimed(fabric);
	multi_count_end = clock();

	printf(B_GREEN ">" B_WHITE " counted inches with multiple claims " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (multi_count_end - multi_count_start) * 1000.0 / CLOCKS_PER_SEC);

	overlap_start = clock();
	for (int a = 0; a < NUM_ENTRIES; a++) {
		if (!overlaps(fabric, entries[a])) {
			non_overlap_claim = entries[a].claim;
			break;
		}
	}
	overlap_end = clock();

	printf(B_GREEN ">" B_WHITE " found non-overlapping claim " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (overlap_end - overlap_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

    // print results
    printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "overlapped inches" WHITE "\t: " CYAN "%d\n" RESET, count);
    printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "non-overlapping claim" WHITE "\t: " CYAN "%d\n" RESET, non_overlap_claim);

	// free fabric array (1MB)
	for (int x = 0; x < FABRIC_SIZE; x++) { free(fabric[x]); }
	free(fabric);

	// free other memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
