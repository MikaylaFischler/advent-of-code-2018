#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include "main.h"

#define LINE_LENGTH 26

typedef struct input_line {
	char* text;
	struct input_line* next;
} input_line_t;

/**
 * @brief Checks if the two strings differ by one character
 * @param str1 the first string
 * @param str2 the second string
 * @return 1 if only one difference, 0 otherwise
 */
uint8_t differs_by_one(char* str1, char* str2) {
	uint8_t diffs = 0;
	for (uint8_t i = 0; i < LINE_LENGTH; i++) {
		if (str1[i] != str2[i]) { diffs++; }
		if (diffs > 1) { return 0; }
	}
	return (diffs == 1);
}

/**
 * @brief Searches for the answer (a string that differs from compare_to by one character)
 * @param head the head of the input lines
 * @param compare_to the string to comare against
 * @return pointer to the string found in the input line that differs by one from the compare_to
 */
char* search_for_answer(input_line_t* head, char* compare_to) {
	if (head) {
		if (differs_by_one(head->text, compare_to)) {
			return head->text;
		} else {
			search_for_answer(head->next, compare_to);
		}
	} else { return NULL; }
}

/**
 * @brief Recursively free input line structs
 * @param head the head of the linked list
 */
void recursive_free(input_line_t* head) {
	if (head) {
		recursive_free(head->next);
		free(head->text);
		free(head);
	}
}

/**
 * @brief Get the string of the common characters between two lines (26 character strings)
 * @param str1 the first line
 * @param str2 the second line
 * @return pointer (that needs to be free'd) to the string of common characters
 */
char* get_common_chars(char* str1, char* str2) {
	uint8_t j = 0;
	char* result = malloc(sizeof(char) * LINE_LENGTH);

	for (uint8_t i = 0; i < LINE_LENGTH; i++) {
		if (str1[i] == str2[i]) {
			result[j++] = str1[i];
		}
	}

	result[LINE_LENGTH] = '\0';

	return result;
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 2" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	uint8_t* seen_count = calloc(sizeof(uint8_t), LINE_LENGTH);

	uint32_t num_with_two = 0;
	uint32_t num_with_three = 0;
	uint8_t found_two = 0;
	uint8_t found_three = 0;
	char* answer_a = NULL;
	char* answer_b = calloc(sizeof(uint8_t), LINE_LENGTH + 1);
	uint8_t p2_answer_found = 0;

	input_line_t* head = calloc(sizeof(input_line_t), 1);
	head->text = calloc(sizeof(char), LINE_LENGTH + 1);
	input_line_t* tail = head;

    clock_t time_start, time_end;

    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

    printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	while (num_read != -1) {
        if ((num_read = getline(&line, &len, fp)) != -1) {
			for (int i = 0; i < len; i++) {
				if (line[i] >= 0x61 && line[i] <= 0x7A) {
					seen_count[line[i] - 0x61]++;
				}
			}

			for (int i = 0; i < LINE_LENGTH; i++) {
				if (seen_count[i] == 2 && !found_two) {
					num_with_two++;
					found_two = 1;
				} else if (seen_count[i] == 3 && !found_three) {
					num_with_three++;
					found_three = 1;
				}

				seen_count[i] = 0;
			}

			found_two = 0;
			found_three = 0;

			if (!p2_answer_found && (answer_a = search_for_answer(head, line))) {
				strcpy(answer_b, line);
				p2_answer_found = 1;
			} else {
				input_line_t* entry = malloc(sizeof(input_line_t));
				entry->text = malloc(sizeof(char) * LINE_LENGTH);
				strcpy(entry->text, line);
				entry->next = NULL;
				tail->next = entry;
				tail = entry;
			}
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	answer_a[26] = '\0';
	answer_b[26] = '\0';

	char* p2_solution = get_common_chars(answer_a, answer_b);

    // print results
    printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
    printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "checksum" WHITE "\t\t: " CYAN "%d * %d = %d\n" RESET, num_with_two, num_with_three, num_with_two * num_with_three);
    printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "differing strings" WHITE "\t: " CYAN "%s and %s\n" RESET, answer_a, answer_b);
    printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "common characters" WHITE "\t: " CYAN "%s\n" RESET, p2_solution);

	fclose(fp);
	if (line) { free(line); }
	free(seen_count);
    free(p2_solution);
	recursive_free(head);

	return 0;
}
