#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define NUM_PLAYERS 416
#define LAST_MARBLE_PTS 71975

typedef struct marble {
	uint64_t value;
	struct marble* next;
	struct marble* prev;
} marble_t;

/**
 * @brief play a marble
 * @param current the current marble
 * @param marble the new marble to play
 * @return the new current marble
 */
marble_t* play_marble(marble_t* current, uint64_t marble) {
	marble_t* after = current->next->next;
	marble_t* before = current->next;
	marble_t* new = malloc(sizeof(marble_t));
	new->value = marble;
	new->next = after;
	after->prev = new;
	new->prev = before;
	before->next = new;
	return new;
}

/**
 * @brief remove the proper marble if someone got a multiple of 23
 * @param current the current marble
 * @return the marble that was removed
 */
marble_t* remove_marble(marble_t* current) {
	int n = 0;
	marble_t* to_remove = current;
	while (n++ < 7) { to_remove = to_remove->prev; }
	marble_t* after = to_remove->next;
	marble_t* before = to_remove->prev;
	before->next = after;
	after->prev = before;
	return to_remove;
}

/**
 * @brief print the marbles
 * @param start the marble to start with
 */
void print_marbles(marble_t* start) {
	marble_t* marble = start;
	do {
		printf("%ld ", marble->value);
		marble = marble->next;
	} while (marble != start);
	printf("\n");
}

void free_marbles(marble_t* marble) {
	marble_t* next = NULL;
    marble->prev->next = NULL;
	while (marble) {
		next = marble->next;
		free(marble);
		marble = next;
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 9" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// configuration
	uint32_t num_players = NUM_PLAYERS;
	uint32_t last_marble = LAST_MARBLE_PTS;

	if (argc == 2 || argc > 3) {
		printf(B_WHITE "Usage: " B_BLUE "./sol" B_RED" NUM_PLAYERS LAST_MARBLE\n");
		printf(WHITE "\tNUM_PLAYERS: the number of players\n");
		printf("\tLAST_MARBLE: the number of the last number\n" RESET);
		return 0;
	} else if (argc == 3) {
		num_players = abs(atoi(argv[1]));
		last_marble = abs(atoi(argv[2]));
	}

	// timing
	clock_t time_start, time_end, since_p1;

	// root marble
	marble_t* current_marble = malloc(sizeof(marble_t));
	current_marble->value = 0;
	current_marble->next = current_marble;
	current_marble->prev = current_marble;

	// game variables
	uint64_t marble = 1;
	uint64_t high_score_p1 = 0;
	uint64_t high_score = 0;
	uint64_t part_2_last_marble = last_marble * 100;
	uint32_t* players = calloc(sizeof(uint32_t), num_players);
	uint16_t player_idx = 0;
	marble_t* removed;

	printf(B_WHITE "number of players : " GREEN "%d\n" RESET, num_players);
	printf(B_WHITE "last marble value : " GREEN "%d\n\n" RESET, last_marble);

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while (marble != part_2_last_marble) {
		if (marble == last_marble) {
			printf(B_GREEN ">" B_WHITE " found part 1 high score " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - time_start) * 1000.0 / CLOCKS_PER_SEC);
			high_score_p1 = high_score;
			since_p1 = clock();
		}

		if (marble > 0 && marble % 23 == 0) {
			removed = remove_marble(current_marble);
			players[player_idx] += marble + removed->value;
			current_marble = removed->next;
			free(removed);

			if (players[player_idx] > high_score) {
				high_score = players[player_idx];
			}
		} else {
			current_marble = play_marble(current_marble, marble);
		}

		marble++;

		if (player_idx < num_players - 1) {
			player_idx++;
		} else { player_idx = 0; }
	}

	time_end = clock();

	printf(B_GREEN ">" B_WHITE " found part 2 high score " WHITE "(additional " BLUE "%.3f ms" WHITE ")\n" RESET, (time_end - since_p1) * 1000.0 / CLOCKS_PER_SEC);

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "high score" WHITE "\t\t: " CYAN "%ld\n" RESET, high_score_p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "high score for 100x" WHITE "\t: " CYAN "%ld\n" RESET, high_score);

	// free up memory
	free(players);
    free_marbles(current_marble);

	return 0;
}
