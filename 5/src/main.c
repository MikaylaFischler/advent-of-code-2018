#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>

#include "main.h"

#define POLYMER_HEAD 0x02
#define POLYMER_TAIL 0x03

typedef struct polymer {
	char type;
	char anti;
	struct polymer* prev;
	struct polymer* next;
} polymer_t;

char anti(char polymer) {
	if (polymer > 0x60) {
		return polymer - 0x20;
	} else {
		return polymer + 0x20;
	}
}

uint8_t polymer_react(polymer_t* head, polymer_t* ptr) {
	polymer_t* next = ptr->next;
	if (next->type != POLYMER_TAIL) {
		if (ptr->anti == next->type) {
			// <-- a - ptr - next - b -->
			polymer_t* a = ptr->prev;
			polymer_t* b = next->next;
			ptr = NULL;
			next = NULL;
			free(ptr);
			free(next);
			a->next = b;
			b->prev = a;
			return polymer_react(head, b) || 1;
		} else { return polymer_react(head, next); }
	} else { return 0; }
}

uint32_t polymer_length(polymer_t* polymer) {
	if (polymer->type != POLYMER_TAIL) {
		return 1 + polymer_length(polymer->next);
	} else { return 0; }
}

uint32_t reverse_polymer_length(polymer_t* polymer) {
	if (polymer->type != POLYMER_HEAD) {
		return 1 + reverse_polymer_length(polymer->prev);
	} else { return 0; }
}

uint8_t polymer_intact(polymer_t* head, polymer_t* tail) {
	return polymer_length(head->next) == reverse_polymer_length(tail->prev);
}

polymer_t* polymer_create(void) {
	polymer_t* polymer_head = malloc(sizeof(polymer_t));
	polymer_head->type = POLYMER_HEAD;
	polymer_head->anti = 0x00;
	polymer_head->prev = NULL;
	polymer_head->next = NULL;
	return polymer_head;
}

void polymer_terminate(polymer_t** tail) {
	polymer_t* t = malloc(sizeof(polymer_t));
	t->type = POLYMER_TAIL;
	t->anti = 0x00;
	t->prev = *tail;
	t->next = NULL;
	(*tail)->next = t;
	(*tail) = t;
}

void polymer_copy(polymer_t* polymer, polymer_t* dest_polymer, char exclude) {
	polymer_t* p;
	polymer_t* polymer_tail = dest_polymer;
	while (p = polymer->next) {
		if (p->type != exclude && p->anti != exclude) {
			polymer_t* cpy = malloc(sizeof(polymer_t));
			cpy->type = p->type;
			cpy->anti = p->anti;
			cpy->prev = polymer_tail;
			cpy->next = NULL;
			polymer_tail->next = cpy;
			polymer_tail = cpy;
		}
		polymer = polymer->next;
	}
	polymer_terminate(&polymer_tail);
}

void polymer_free(polymer_t* polymer) {
	if (polymer->next) { polymer_free(polymer->next); }
	free(polymer);
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 5" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
	char polymer;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, assemble_start, assemble_end, full_start,
		full_end, reduced_start, reduced_end;

	// polymer variables
	polymer_t* polymer_backup = polymer_create();
	polymer_t* polymer_head = polymer_create();
	polymer_t* polymer_tail = polymer_head;

	printf(YELLOW "starting...\n" RESET);

	assemble_start = time_start = clock();

	// assemble polymer chain
	while ((polymer = fgetc(fp)) != EOF) {
		if (!isspace(polymer)) {
			polymer_t* p = malloc(sizeof(polymer_t));
			p->type = polymer;
			p->anti = anti(polymer);
			p->prev = polymer_tail;
			p->next = NULL;
			polymer_tail->next = p;
			polymer_tail = p;
		}
	}

	// terminate and duplicate the chain
	polymer_terminate(&polymer_tail);
	polymer_copy(polymer_head, polymer_backup, 0x00);
	assemble_end = clock();
	printf(B_GREEN ">" B_WHITE " assembled polymer " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (assemble_end - assemble_start) * 1000.0 / CLOCKS_PER_SEC);

	// polymer_react polymer chain
	uint32_t count = 0;

	printf(YELLOW ">" B_WHITE " reacting full polymer\n" RESET);
	full_start = clock();
	while (polymer_react(polymer_head, polymer_head->next)) { count++; }
	full_end = clock();
	printf(B_GREEN ">" B_WHITE " reacted full polymer " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (full_end - full_start) * 1000.0 / CLOCKS_PER_SEC);

	// measure and dispose of the polymer
	uint32_t length = polymer_length(polymer_head->next);
	polymer_free(polymer_head);

	printf(YELLOW ">" B_WHITE " reacting reduced polymers...\n" RESET);
	uint32_t reduced_lengths[26];
	for (int i = 0x41; i < 0x5B; i++) {
		reduced_start = clock();
		polymer_head = polymer_create();
		polymer_copy(polymer_backup, polymer_head, i);
		while (polymer_react(polymer_head, polymer_head->next));
		reduced_lengths[i - 0x41] = polymer_length(polymer_head->next);
		polymer_free(polymer_head);
		reduced_end = clock();
		printf(RED ">" B_WHITE " removed %c: " CYAN "%d " WHITE "(%.3f ms)\n" RESET, i, reduced_lengths[i - 0x41], (reduced_end - reduced_start) * 1000.0 / CLOCKS_PER_SEC);
	}

	uint32_t min_length = length;
	char to_remove = ' ';
	for (int i = 0; i < 26; i++) {
		if (reduced_lengths[i] < min_length) {
			min_length = reduced_lengths[i];
			to_remove = i + 0x41;
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "units left from full polymer reaction" WHITE "\t: " CYAN "%d after %d polymer reactions\n" RESET, length, count);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "removal of this unit for most reaction" WHITE "\t: " CYAN "%c resulting in length of %d\n" RESET, to_remove, reduced_lengths[to_remove - 0x41]);

	// free up memory
	fclose(fp);

	return 0;
}
