#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

// #define DEBUG

#define FALL_ASLEEP 0x1
#define WAKE_UP 0x2
#define GUARD_ID 0x4

typedef struct event {
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t type;
	uint16_t guard_id;	// will be 0 (null) if not a GUARD_ID type
	struct event* before;
	struct event* after;
} event_t;

typedef struct guard {
	uint16_t id;
	uint32_t minutes_slept;
	uint8_t* minute_asleep;
	struct guard* next;
} guard_t;

/**
 * @brief get the weight value for sorting
 * @param event the event to get the weight of
 * @return the weight of the event
 */
uint32_t get_weight(event_t* event) {
	return event->minute + (event->hour * 100) + (event->day * 10000) + (event->month * 1000000);
}

/**
 * @brief put the event into a sorted list (builds up a sorted list)
 * @param base the base (some point in the list) to start at
 * @param event the event to insert
 */
void insertion_sort_insert(event_t* base, event_t* event) {
	uint32_t base_weight = get_weight(base);
	uint32_t event_weight = get_weight(event);

	if (event_weight > base_weight) {
		if (base->after) {
			uint32_t base_a_weight = get_weight(base->after);
			if (event_weight < base_a_weight) {
				base->after->before = event;
				event->after = base->after;
				event->before = base;
				base->after = event;
			} else {
				insertion_sort_insert(base->after, event);
			}
		} else {
			event->before = base;
			base->after = event;
		}
	} else if (event_weight < base_weight) {
		if (base->before) {
			uint32_t base_b_weight = get_weight(base->before);
			if (event_weight > base_b_weight) {
				base->before->after = event;
				event->before = base->before;
				event->after = base;
				base->before = event;
			} else {
				insertion_sort_insert(base->before, event);
			}
		} else {
			event->after = base;
			base->before = event;
		}
	}
}

/**
 * @brief find the start of the list (earliest event)
 * @param event a starting point
 * @return starting event
 */
event_t* find_start(event_t* event) {
	if (event->before) {
		return find_start(event->before);
	} else { return event; }
}

/**
 * @brief find a guard with the given ID or add a guard with given ID
 * @param guard the guard to start at (should initially be the head of the linked list)
 * @param id the ID to search for or insert
 * @return the guard object (should never be null)
 */
guard_t* add_or_find_guard(guard_t* guard, uint16_t id) {
	if (guard->id == id) {
		return guard;
	} else if (guard->next) {
		return add_or_find_guard(guard->next, id);
	} else {
		guard_t* g = malloc(sizeof(guard_t));
		g->id = id;
		g->minutes_slept = 0;
		g->minute_asleep = calloc(sizeof(uint16_t), 60);
		g->next = NULL;
		guard->next = g;
		return g;
	}
}

/**
 * @brief find the guard with most time asleep
 * @param guard the guard to start with (the head of the list)
 * @return the guard that is asleep most
 */
guard_t* find_most_asleep(guard_t* guard) {
	if (guard) {
		guard_t* guard_b = find_most_asleep(guard->next);
		uint16_t compare_to = (guard_b) ? guard_b->minutes_slept : 0;

		if (guard->minutes_slept >= compare_to) {
			return guard;
		} else { return guard_b; }
	} else { return NULL; }
}

/**
 * @brief recursively free events
 * @param start event to start with (should not be NULL)
 */
void recursive_free_events(event_t* start) {
	if (start->after) { recursive_free_events(start->after); }
	free(start);
}

/**
 * @brief recursively free guards
 * @param guard the guard to start with (should not be NULL)
 */
void recursive_free_guards(guard_t* guard) {
	if (guard->next) { recursive_free_guards(guard->next); }
	free(guard);
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 4" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// string temp vars
	char id_str[5];
	char month_str[3];
	char day_str[3];
	char hour_str[3];
	char minute_str[3];

	// values for parsing in input file
	uint32_t line_id = 0;
	uint8_t state = 0;
	uint8_t type = 0;
	uint8_t x = 0;

	// linked lists/pointers
	event_t* base = NULL;
	event_t* start = NULL;
	guard_t* guards = calloc(sizeof(guard_t), 1);

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// check verbosity
	uint8_t verbose = !(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'q');

	// timing variables
	clock_t time_start, time_end, parse_start, parse_end, stat_start, stat_end,
		p1_start, p1_end, p2_start, p2_end;

    printf(YELLOW "starting...\n" RESET);

	parse_start = time_start = clock();

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = type = 0;
		for (int i = 0; i < len; i++) {
			switch (state) {
				case 0:
					if (line[i] == '-') { state++; }
					break;
				case 1:
					if (line[i] == '-') {
						state++;
						month_str[x] = '\0';
						x = 0;
					} else { month_str[x++] = line[i]; }
					break;
				case 2:
					if (line[i] == ' ') {
						state++;
						day_str[x] = '\0';
						x = 0;
					} else { day_str[x++] = line[i]; }
					break;
				case 3:
					if (line[i] == ':') {
						state++;
						hour_str[x] = '\0';
						x = 0;
					} else { hour_str[x++] = line[i]; }
					break;
				case 4:
					if (line[i] == ']') {
						state++;
						minute_str[x] = '\0';
						x = 0;
					} else { minute_str[x++] = line[i]; }
					break;
				case 5:
					if (line[i] == '#') {
						type = GUARD_ID;
						state++;
					} else if (line[i] == 'w') {
						type = WAKE_UP;
						break;
					} else if (line[i] == 'l') {
						type = FALL_ASLEEP;
						break;
					}
					break;
				case 6:
					if (line[i] == ' ') {
						state++;
						id_str[x] = '\0';
						x = 0;
					} else { id_str[x++] = line[i]; }
					break;
			}
		}

		if ((state == 5 && type != GUARD_ID) || (state == 7 && type == GUARD_ID)) {
			event_t* event = malloc(sizeof(event_t));
			event->month = atoi(month_str);
			event->day = atoi(day_str);
			event->hour = atoi(hour_str);
			event->minute = atoi(minute_str);
			event->type = type;
			event->guard_id = (type == GUARD_ID) ? atoi(id_str) : 0;
			event->before = event->after = NULL;

			if (base) {
				insertion_sort_insert(base, event);
			} else { base = event; }
		}
	}

	if (verbose) {
		parse_end = clock();
		printf(B_GREEN ">" B_WHITE " parsed input into sorted list " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (parse_end - parse_start) * 1000.0 / CLOCKS_PER_SEC);
	}

	// get the first event
	start = find_start(base);

	// determine sleep statistics for guards
	event_t* e = start;
	guard_t* guard = NULL;
	uint8_t start_min = 0;
	uint8_t guard_state = 0;

	if (verbose) { stat_start = clock(); }

	while (e) {
		#ifdef DEBUG
		printf(YELLOW ">" WHITE " %#lx (%#lx<>%#lx): %d-%d %d:%d (type=%d)\n" RESET, (size_t) e, (size_t) e->before, (size_t) e->after, e->month, e->day, e->hour, e->minute, e->type);
		#endif

		if (e->type == GUARD_ID) {
			guard_state = 0;
			guard = add_or_find_guard(guards, e->guard_id);
			// printf("%#lx: %d,%d\n", (size_t) guard, guard->id, guard->minutes_slept);
		} else if (guard) {
			if (e->type == FALL_ASLEEP) {
				start_min = e->minute;
				guard_state = FALL_ASLEEP;
			} else if (e->type == WAKE_UP) {
				guard_state = WAKE_UP;
				guard->minutes_slept += (e->minute - start_min);
				for (int i = start_min; i < e->minute; i++) { guard->minute_asleep[i]++; }
			}
		}

		e = e->after;
	}

	if (verbose) {
		stat_end = clock();
		printf(B_GREEN ">" B_WHITE " set time asleep and minutes where asleep for guards " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (stat_end - stat_start) * 1000.0 / CLOCKS_PER_SEC);
	}

	// find hour where the most often sleeping guard is mostly asleep
	uint16_t max = 0;
	uint8_t max_minute = 0;
	guard_t* sleepy = find_most_asleep(guards);

	if (verbose) { p1_start = clock(); }

	for (int i = 0; i < 60; i++) {
		if (sleepy->minute_asleep[i] >= max) {
			max = sleepy->minute_asleep[i];
			max_minute = i;
		}
	}

	if (verbose) {
		p1_end = clock();
		printf(B_MAGENTA ">" B_WHITE " determined which minute guard with most sleep is asleep " WHITE "(" BLUE "%ld us" WHITE ")\n" RESET, (p1_end - p1_start) * 1000000 / CLOCKS_PER_SEC);
	}

	// find guard most often asleep at at given minute
	uint32_t _max = 0;
	uint8_t _max_minute = 0;
	guard_t* guard_most_asleep_at_time = NULL;
	guard_t* g = guards->next;

	if (verbose) { p2_start = clock(); }

	while (g) {
		for (int i = 0; i < 60; i++) {
			if (g->minute_asleep[i] >= _max) {
				_max = g->minute_asleep[i];
				_max_minute = i;
				guard_most_asleep_at_time = g;
			}
		}

		g = g->next;
	}

	if (verbose) {
		p2_end = clock();
		printf(B_MAGENTA ">" B_WHITE " determined which guard has most time asleep in a any minute " WHITE "(" BLUE "%ld us" WHITE ")\n" RESET, (p2_end - p2_start) * 1000000 / CLOCKS_PER_SEC);
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t\t\t\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "when guard with most sleep is most often asleep" WHITE "\t: " CYAN "Guard #%d in minute %d\n" RESET, sleepy->id, max_minute);
    printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "guard who is most often asleep in a minute" WHITE "\t\t: " CYAN "Guard #%d in minute %d\n" RESET, guard_most_asleep_at_time->id, _max_minute);

	// free up memory
	fclose(fp);
	if (line) { free(line); }
	recursive_free_events(start);
	recursive_free_guards(guards);

	return 0;
}
