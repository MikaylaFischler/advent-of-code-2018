#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>

#include "main.h"

#define TRAIN_UP '^'
#define TRAIN_RIGHT '>'
#define TRAIN_DOWN 'v'
#define TRAIN_LEFT '<'

#define TURN_LEFT 0
#define GO_STRAIGHT 1
#define TURN_RIGHT 2

#define TRACK_NONE ' '
#define TRACK_VERT '|'
#define TRACK_HORIZ '-'
#define TRACK_CURV_A '/'
#define TRACK_CURV_B '\\'
#define TRACK_INTERSECT '+'

#define WIDTH 150
#define HEIGHT 150

int lazy_count = 17;

typedef struct train {
	uint8_t x;
	uint8_t y;
	uint8_t direction;
	uint8_t next_instr;
	uint8_t moved;
} train_t;

typedef struct track {
	uint8_t x;
	uint8_t y;
	uint8_t type;
	struct train* train;
} track_t;

uint8_t is_train(char c) {
	if (c == TRAIN_UP || c == TRAIN_RIGHT || c == TRAIN_DOWN || c == TRAIN_LEFT) {
		return c;
	} else { return 0; }
}

uint8_t is_track(char c) {
	if (c == TRACK_VERT || c == TRACK_HORIZ || c == TRACK_CURV_A || c == TRACK_CURV_B || c == TRACK_INTERSECT) {
		return c;
	} else { return 0; }
}

uint8_t iterate_system(track_t*** map) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (map[y][x] && map[y][x]->train && map[y][x]->train->moved == 0) {
				track_t* track = map[y][x];
				train_t* train = track->train;

				train->moved = 1;

				switch (train->direction) {
					case TRAIN_UP:
						if (map[y-1][x] == NULL) { printf(RED "CORRUPTED\n" RESET); }
						if (map[y-1][x]->train) {
							printf("crash at %d,%d!\n", x, y-1);
							free(train);
							free(map[y-1][x]->train);
							track->train = NULL;
							map[y-1][x]->train = NULL;
							lazy_count-=2;
						} else {
							switch (map[y-1][x]->type) {
								case TRACK_CURV_A:
									train->direction = TRAIN_RIGHT;
									break;
								case TRACK_CURV_B:
									train->direction = TRAIN_LEFT;
									break;
								case TRACK_INTERSECT:
									switch (train->next_instr) {
										case TURN_LEFT:
											train->direction = TRAIN_LEFT;
											train->next_instr++;
											break;
										case GO_STRAIGHT:
											train->next_instr++;
											break;
										case TURN_RIGHT:
											train->direction = TRAIN_RIGHT;
											train->next_instr = TURN_LEFT;
											break;
									}
									break;
								case TRACK_HORIZ: printf(RED "CORRUPTED\n" RESET);
							}

							map[y-1][x]->train = train;
							track->train = NULL;
						}
						break;
					case TRAIN_RIGHT:
						if (map[y][x+1] == NULL) { printf(RED "CORRUPTED\n" RESET); }
						if (map[y][x+1]->train) {
							printf("crash at %d,%d!\n", x+1, y);
							free(train);
							free(map[y][x+1]->train);
							track->train = NULL;
							map[y][x+1]->train = NULL;
							lazy_count-=2;
						} else {
							switch (map[y][x+1]->type) {
								case TRACK_CURV_A:
									train->direction = TRAIN_UP;
									break;
								case TRACK_CURV_B:
									train->direction = TRAIN_DOWN;
									break;
								case TRACK_INTERSECT:
									switch (train->next_instr) {
										case TURN_LEFT:
											train->direction = TRAIN_UP;
											train->next_instr++;
											break;
										case GO_STRAIGHT:
											train->next_instr++;
											break;
										case TURN_RIGHT:
											train->direction = TRAIN_DOWN;
											train->next_instr = TURN_LEFT;
											break;
									}
									break;
								case TRACK_VERT: printf(RED "CORRUPTED\n" RESET);
							}

							map[y][x+1]->train = train;
							track->train = NULL;
						}
						break;
					case TRAIN_DOWN:
						if (map[y+1][x] == NULL) { printf(RED "CORRUPTED\n" RESET); }
						if (map[y+1][x]->train) {
							printf("crash at %d,%d!\n", x, y+1);
							free(train);
							free(map[y+1][x]->train);
							track->train = NULL;
							map[y+1][x]->train = NULL;
							lazy_count-=2;
						} else {
							switch (map[y+1][x]->type) {
								case TRACK_CURV_A:
									train->direction = TRAIN_LEFT;
									break;
								case TRACK_CURV_B:
									train->direction = TRAIN_RIGHT;
									break;
								case TRACK_INTERSECT:
									switch (train->next_instr) {
										case TURN_LEFT:
											train->direction = TRAIN_RIGHT;
											train->next_instr++;
											break;
										case GO_STRAIGHT:
											train->next_instr++;
											break;
										case TURN_RIGHT:
											train->direction = TRAIN_LEFT;
											train->next_instr = TURN_LEFT;
											break;
									}
									break;
								case TRACK_HORIZ: printf(RED "CORRUPTED\n" RESET);
							}

							map[y+1][x]->train = train;
							track->train = NULL;
						}
						break;
					case TRAIN_LEFT:
						if (map[y][x-1] == NULL) { printf(RED "CORRUPTED\n" RESET); }
						if (map[y][x-1]->train) {
							printf("crash at %d,%d!\n", x-1, y);
							free(train);
							free(map[y][x-1]->train);
							track->train = NULL;
							map[y][x-1]->train = NULL;
							lazy_count-=2;
						} else {
							switch (map[y][x-1]->type) {
								case TRACK_CURV_A:
									train->direction = TRAIN_DOWN;
									break;
								case TRACK_CURV_B:
									train->direction = TRAIN_UP;
									break;
								case TRACK_INTERSECT:
									switch (train->next_instr) {
										case TURN_LEFT:
											train->direction = TRAIN_DOWN;
											train->next_instr++;
											break;
										case GO_STRAIGHT:
											train->next_instr++;
											break;
										case TURN_RIGHT:
											train->direction = TRAIN_UP;
											train->next_instr = TURN_LEFT;
											break;
									}
									break;
								case TRACK_VERT: printf(RED "CORRUPTED\n" RESET);
							}

							map[y][x-1]->train = train;
							track->train = NULL;
						}

						break;
				}
			}
		}
	}

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (map[y][x] && map[y][x]->train) {
				map[y][x]->train->moved = 0;
			}
		}
	}

	return lazy_count < 2;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 13" WHITE "           |\n" RESET);
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

	char c;

	track_t*** map = calloc(sizeof(track_t**), HEIGHT);
	for (int y = 0; y < HEIGHT; y++) {
		map[y] = calloc(sizeof(track_t**), WIDTH);
	}

	track_t* track;
	train_t* train;
	uint8_t y = 0;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		for (int x = 0; x < num_read - 1; x++) {
			c = line[x];
			// printf("%c", c);

			if (c == ' ') {
				map[y][x] = NULL;
			} else {
				track = malloc(sizeof(track_t));
				track->x = x;
				track->y = y;

				map[y][x] = track;

				if (is_train(c)) {
					train = malloc(sizeof(train_t));
					train->direction = c;
					train->x = x;
					train->y = y;
					train->next_instr = TURN_LEFT;
					train->moved = 0;
					track->train = train;

					if (c == '^' || c == 'v') {
						track->type = TRACK_VERT;
					} else {
						track->type = TRACK_HORIZ;
					}
				} else if (is_track(c)) {
					track->type = c;
					track->train = NULL;
				}
			}
		}
		// printf("\n");
		y++;
	}
	//
	// for (int y = 0; y < HEIGHT; y++) {
	// 	for (int x = 0; x < WIDTH; x++) {
	// 		if (map[y][x] && map[y][x]->train) {
	// 			printf(RED "%c" RESET, map[y][x]->train->direction);
	// 		} else if (map[y][x]) {
	// 			printf(B_BLACK "%c" RESET, map[y][x]->type);
	// 		} else { printf(" "); }
	// 	}
	// 	printf("\n");
	// }

	uint8_t crash = 0;

	while (crash == 0) {
		crash = iterate_system(map);

		// for (int y = 0; y < 50; y++) {
		// 	for (int x = 0; x < WIDTH; x++) {
		// 		if (map[y][x] && map[y][x]->train) {
		// 			printf(RED "%c" RESET, map[y][x]->train->direction);
		// 		} else if (map[y][x]) {
		// 			printf(B_BLACK "%c" RESET, map[y][x]->type);
		// 		} else { printf(" "); }
		// 	}
		// 	printf("\n");
		// }

		// sleep(1);

	}



	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (map[y][x] && map[y][x]->train) {
				printf(BLUE "last cart at %d,%d\n" RESET, x, y);
			}
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
