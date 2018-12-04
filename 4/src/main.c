#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "main.h"

#define FALL_ASLEEP 0x1
#define WAKE_UP 0x2
#define GUARD_ID 0x4

#define IS_ASLEEP 0x10

typedef struct event {
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t type;
	uint16_t guard_id;	// might be 0 (null)
	struct event* before;
	struct event* after;
} event_t;

typedef struct guard {
	uint16_t id;
	uint32_t minutes_slept;
	uint8_t* minute_asleep;
	struct guard* next;
} guard_t;

uint32_t get_weight(event_t* event) {
	return event->minute + (event->hour * 100) + (event->day * 10000) + (event->month * 1000000);
}

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

event_t* find_start(event_t* event) {
	if (event->before) {
		return find_start(event->before);
	} else {
		return event;
	}
}

guard_t* add_or_find_guard(guard_t* guard, uint16_t id) {
	if (guard->id == id) {
		return guard;
	} else if (guard->next) {
		return add_or_find_guard(guard->next, id);
	} else {
		guard_t* g = calloc(sizeof(guard_t), 1);
		g->id = id;
		g->minutes_slept = 0;
		g->minute_asleep = calloc(sizeof(uint16_t), 60);
		g->next = NULL;
		guard->next = g;
		return g;
	}
}

guard_t* find_most_asleep(guard_t* guard) {
	if (guard) {
		guard_t* guard_b = find_most_asleep(guard->next);
		uint16_t compare_to = (guard_b) ? guard_b->minutes_slept : 0;
		printf(RED "COMPARE %d >= %d\n" RESET, guard->minutes_slept, compare_to);
		if (guard->minutes_slept >= compare_to) {
			return guard;
		} else { return guard_b; }
	} else { return NULL; }
}

void recursive_free_events(event_t* start) {
	if (start->after) { recursive_free_events(start->after); }
	free(start);
}

void recursive_free_guards(guard_t* guard) {
	if (guard->next) { recursive_free_guards(guard->next); }
	free(guard);
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 4" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

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

	uint8_t type = 0;


	uint8_t state = 0;
	uint8_t x = 0;

	event_t* base = NULL;
	event_t* start = NULL;
	guard_t* guards = calloc(sizeof(guard_t), 1);

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

    printf(YELLOW "starting...\n" RESET);

	uint32_t line_id = 0;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		state = 0;
		type = 0;
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

	start = find_start(base);

	event_t* e = start;
	guard_t* guard = NULL;
	uint8_t start_min = 0;
	uint8_t guard_state = 0;

	while (e) {
		printf("%#lx (%#lx<>%#lx): %d-%d %d:%d (type=%d)\n", (size_t) e, (size_t) e->before, (size_t) e->after, e->month, e->day, e->hour, e->minute, e->type);

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
			}

			if (guard_state && FALL_ASLEEP) {
				guard->minute_asleep[e->minute]++;
			}
		}

		e = e->after;
	}

	guard_t* sleepy = find_most_asleep(guards);
	printf(GREEN "%d (%d)\n" RESET, sleepy->id, sleepy->minutes_slept);

	uint16_t max = 0;
	uint8_t max_hour = 0;
	for (int i = 0; i < 60; i++) {
		if (sleepy->minute_asleep[i] >= max) {
			max = sleepy->minute_asleep[i];
			max_hour = i;
			// printf(GREEN "%d,%d\n" RESET, max, max_hour);
		}
		printf(GREEN "%d,%d\n" RESET, sleepy->minute_asleep[i], i);
	}

	printf(GREEN "%d,%d\n" RESET, max, max_hour);



	recursive_free_events(start);
	recursive_free_guards(guards);

	return 0;
}
