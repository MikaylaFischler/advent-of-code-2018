#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

void init_reg(uint16_t* dest, uint16_t* src) {
	for (uint8_t i = 0; i < 4; i++) { dest[i] = src[i]; }
}

uint8_t check_reg(uint16_t* act_reg, uint16_t* exp_reg) {
	for (uint8_t i = 0; i < 4; i++) { if (act_reg[i] != exp_reg[i]) return 0; }
	return 1;
}

uint8_t mapped(uint8_t instr, int8_t* map) {
	for (uint8_t i = 0; i < 16; i++) { if (map[i] == instr) return 1; }
	return 0;
}

void _addr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] + reg[b]; }
void _addi(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] + b; }

void _mulr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] * reg[b]; }
void _muli(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] * b; }

void _banr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] & reg[b]; }
void _bani(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] & b; }

void _borr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] | reg[b]; }
void _bori(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] | b; }

void _setr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a]; }
void _seti(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = a; }

void _gtir(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = a > reg[b]; }
void _gtri(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] > b; }
void _gtrr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] > reg[b]; }

void _eqir(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = a == reg[b]; }
void _eqri(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] == b; }
void _eqrr(uint8_t a, uint8_t b, uint8_t c, uint16_t* reg) { reg[c] = reg[a] == reg[b]; }

void (*instr_set[16]) (uint8_t, uint8_t, uint8_t, uint16_t*);

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 16" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// instruction set

	instr_set[0]  = _addr;
	instr_set[1]  = _addi;
	instr_set[2]  = _mulr;
	instr_set[3]  = _muli;
	instr_set[4]  = _banr;
	instr_set[5]  = _bani;
	instr_set[6]  = _borr;
	instr_set[7]  = _bori;
	instr_set[8]  = _setr;
	instr_set[9]  = _seti;
	instr_set[10] = _gtir;
	instr_set[11] = _gtri;
	instr_set[12] = _gtrr;
	instr_set[13] = _eqir;
	instr_set[14] = _eqri;
	instr_set[15] = _eqrr;

	uint8_t num_mapped = 0;
	int8_t instr_map[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	// timing
	clock_t time_start, time_end;

	uint16_t behaves_like_3_plus = 0;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	uint8_t state = 0;
	uint8_t nl_count = 0;

	uint8_t match_idx = 0;
	uint8_t match = 0;
	uint8_t match_count = 0;

	uint8_t run = 0;

	char temp_str[3] = { 0, 0, 0 };

	uint16_t* start_reg = malloc(sizeof(uint16_t) * 4);
	uint16_t* end_reg = malloc(sizeof(uint16_t) * 4);
	uint16_t* in_reg = malloc(sizeof(uint16_t) * 4);
	uint8_t* instr = malloc(sizeof(uint8_t) * 4);

	while (num_mapped < 16) {
		while ((num_read = getline(&line, &len, fp)) != -1) {
			if (num_read == 1) { nl_count++; } else { nl_count = 0; }

			if (state == 0 && line[0] == 'B') {
				start_reg[0] = line[9] - 0x30;
				start_reg[1] = line[12] - 0x30;
				start_reg[2] = line[15] - 0x30;
				start_reg[3] = line[18] - 0x30;
				state++;
			} else if (state == 1 && line[0] > 0x29 && line[0] < 0x3A) {
				uint8_t i = 0;
				for (; i < 2; i++) {
					if (line[i] == ' ') { break; }
					temp_str[i] = line[i];
				}

				temp_str[i] = '\0';

				instr[0] = atoi(temp_str);
				instr[1] = line[1 + i] - 0x30;
				instr[2] = line[3 + i] - 0x30;
				instr[3] = line[5 + i] - 0x30;
				state++;
			} else if (state == 2 && line[0] == 'A') {
				end_reg[0] = line[9] - 0x30;
				end_reg[1] = line[12] - 0x30;
				end_reg[2] = line[15] - 0x30;
				end_reg[3] = line[18] - 0x30;
				state++;
			}

			if (state == 3) {
				match_idx = 0;
				match = 0;
				match_count = 0;

				for (uint8_t i = 0; i < 16; i++) {
					init_reg(in_reg, start_reg);
					instr_set[i](instr[1], instr[2], instr[3], in_reg);

					if (check_reg(in_reg, end_reg) && !mapped(i, instr_map)) {
						match = instr[0];
						match_idx = i;
						match_count++;
					}
				}

				if (match_count == 1 && run > 0) {
					instr_map[match] = match_idx;
					num_mapped++;
					printf("found exact match: %d for instr index %d.\n", match, match_idx);
				}

				if (match_count > 2 && run == 0) { behaves_like_3_plus++; }

				state = 0;
			}
		}

		run++;
		rewind(fp);
	}

	uint16_t* reg = calloc(sizeof(uint16_t), 4);

	while ((num_read = getline(&line, &len, fp)) != -1) {
		if (state == 1) {
			uint8_t i = 0;
			for (; i < 2; i++) {
				if (line[i] == ' ') { break; }
				temp_str[i] = line[i];
			}

			temp_str[i] = '\0';

			instr[0] = atoi(temp_str);
			instr[1] = line[1 + i] - 0x30;
			instr[2] = line[3 + i] - 0x30;
			instr[3] = line[5 + i] - 0x30;

			// printf(MAGENTA ">exec instr %d\n" RESET, instr_map[instr[0]]);

			instr_set[instr_map[instr[0]]](instr[1], instr[2], instr[3], reg);
		} else {
			if (nl_count == 3) {
				printf(B_BLACK "hit part 2\n" RESET);
				state = 1;
			} else {
				if (num_read == 1) { nl_count++; } else { nl_count = 0; }
			}
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "behaves like 3 or more" WHITE "\t: " CYAN "%d\n" RESET, behaves_like_3_plus);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "register 0 at end" WHITE "\t: " CYAN "%d\n" RESET, reg[0]);

	// free up memory
	free(start_reg);
	free(end_reg);
	free(in_reg);
	free(instr);
	free(reg);

	fclose(fp);
	if (line) { free(line); }

	return 0;
}
