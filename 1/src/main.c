#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

struct frequency {
    int32_t freq;
    struct frequency* left;
    struct frequency* right;
};

/**
 * @brief free's frequency linked list
 * @param sf frequency struct head to start freeing at
 */
void recurse_delete(struct frequency* sf) {
    if (sf) {
        recurse_delete(sf->left);
        recurse_delete(sf->right);
        free(sf);
    }
}

/**
 * @brief recursively searches linked list for given value
 * @param sf frequency struct head
 * @param val value to compare to
 */
uint8_t recurse_find(struct frequency* sf, int32_t val) {
    if (sf) {
        if (sf->freq == val) {
            return 1;
        } else if (sf->freq > val) {
            return recurse_find(sf->left, val);
        } else {
            return recurse_find(sf->right, val);
		}
    } else { return 0; }
}

void append_frequency(struct frequency* head, struct frequency* entry) {
	if (head) {
		if (entry->freq > head->freq) {
			if (head->right == NULL) {
				head->right = entry;
			} else {
				append_frequency(head->right, entry);
			}
		} else {
			if (head->left == NULL) {
				head->left = entry;
			} else {
				append_frequency(head->left, entry);
			}
		}
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 1" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

    FILE* fp;
    char* line = NULL;
    size_t len = 0;

    int32_t freq = 0;
    int32_t part_1_freq = 0;
    uint32_t loop_count = 0;
	uint64_t num_freq = 0;

    uint8_t verbose = !(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'q');

    clock_t total_time, end_total, loop_time, end_loop;

    struct frequency* head = (struct frequency*) calloc(sizeof(struct frequency), 1);
	head->left = NULL;
	head->right = NULL;

    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

    printf(WHITE "verbosity set to " CYAN "%d\n" RESET, verbose);
    printf(B_WHITE "starting...\n\n" RESET);
    total_time = clock();

    if (verbose) {
		printf(WHITE "> loop" B_WHITE "\t%d" WHITE ": " YELLOW "started\n" RESET, loop_count);
		loop_time = clock();
	}

    // interpret input and find solutions
    while (1) {
        if (getline(&line, &len, fp) != -1) {
            // update frequency
            freq += strtol(line, NULL, 10);

            // check if this is a duplicate
            if (recurse_find(head, freq)) {
                printf(WHITE ">> loop" B_WHITE "\t%d" WHITE ": " GREEN "found duplicate\n" RESET, loop_count);
                break;
            }

            // store frequency into history
            struct frequency* tmp = (struct frequency*) malloc(sizeof(struct frequency));
            tmp->freq = freq;
            tmp->left = NULL;
            tmp->right = NULL;

			// insert it
			num_freq++;
			append_frequency(head, tmp);
        } else {
            if (verbose) {
                end_loop = clock();
                printf(WHITE "> loop" B_WHITE "\t%d" WHITE ": " GREEN "completed in %f seconds\n" RESET, loop_count++, (double) (end_loop - loop_time) / CLOCKS_PER_SEC);
            } else { loop_count++; }

            if (loop_count == 1) { part_1_freq = freq; }
            rewind(fp);

            if (verbose) {
                printf(WHITE "> loop" B_WHITE "\t%d" WHITE ": " YELLOW "started\n" RESET, loop_count);
                loop_time = clock();
            }
        }
    }

    end_total = clock();

    // print results
    printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (end_total - total_time) / CLOCKS_PER_SEC);
    printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "loop end freq" WHITE "\t: " CYAN "%d\n" RESET, part_1_freq);
    printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "duplicate" WHITE "\t: " CYAN "%d\n" RESET, freq);

    // memory clean up
    if (line) { free(line); }
    fclose(fp);
    recurse_delete(head);

    // exit
	return 0;
}
