#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

#define NUM_INSTR 31

typedef struct instruction {
	void (*instr) (uint64_t, uint64_t, uint8_t, uint64_t*);
	uint64_t a;
	uint64_t b;
	uint8_t c;
} instruction_t;

/**
 * @brief convert a written instruction to an instruction set index
 * @param instr a pointer to the start of instruction text
 * @return instruction index
 */
uint8_t instr_to_idx(char* instr) {
	char a = instr[0];
	char b = instr[1];
	char c = instr[2];
	char d = instr[3];

	if (a == 'a') {
		return (d == 'r') ? 0 : 1;
	} else if (a == 'm') {
		return (d == 'r') ? 2 : 3;
	} else if (a == 'b') {
		if (b == 'a') {
			return (d == 'r') ? 4 : 5;
		} else {
			return (d == 'r') ? 6 : 7;
		}
	} else if (a == 's') {
		return (d == 'r') ? 8 : 9;
	} else if (a == 'g') {
		if (c == 'i') { return 10; }
		return (d == 'i') ? 11 : 12;
	} else if (a == 'e') {
		if (c == 'i') { return 13; }
		return (d == 'i') ? 14 : 15;
	} else {
		return 16; // error
	}
}

/**
 * @brief run an elfcode program
 * @param instructs the instructions
 * @param reg the register
 * @param ip_reg the index of the instruction pointer register
 * @param p1 pointer to part 1 solution
 * @param p2 pointer to part 2 solution
 */
void run(instruction_t* instructs, uint64_t* reg, uint8_t ip_reg, uint64_t* p1, uint64_t* p2) {
	uint16_t instr_ptr = 0;
	instruction_t instruct;

	uint64_t values[15000];
	for (int i = 0; i < 15000; i++) { values[i] = 0; }
	uint64_t last_val = 0;
	uint32_t val_idx = 0;
	uint8_t found = 0;

	for (; instr_ptr < NUM_INSTR; instr_ptr++) {
		reg[ip_reg] = instr_ptr;
		instruct = instructs[instr_ptr];
		instruct.instr(instruct.a, instruct.b, instruct.c, reg);
		instr_ptr = reg[ip_reg];

		if (instr_ptr == 28) {
			if (!*p1) { *p1 = reg[1]; }

			for (int i = found = 0; i < val_idx; i++) {
				if (reg[1] == values[i]) {
					found = 1;
					break;
				}
			}

			if (found == 0) { values[val_idx++] = last_val = reg[1]; }

			if ((found == 1 && reg[1] > 0)) {
				*p2 = last_val;
				return;
			}
		}

		if (instr_ptr == NUM_INSTR - 1) { break; }
	}
}

/**
 * @brief 64-bit elfcode functions
 */

void _addr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] + reg[b]; }
void _addi(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] + b; }

void _mulr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] * reg[b]; }
void _muli(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] * b; }

void _banr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] & reg[b]; }
void _bani(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] & b; }

void _borr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] | reg[b]; }
void _bori(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] | b; }

void _setr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a]; }
void _seti(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = a; }

void _gtir(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = a > reg[b]; }
void _gtri(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] > b; }
void _gtrr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] > reg[b]; }

void _eqir(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = a == reg[b]; }
void _eqri(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] == b; }
void _eqrr(uint64_t a, uint64_t b, uint8_t c, uint64_t* reg) { reg[c] = reg[a] == reg[b]; }

void (*instr_set[16]) (uint64_t, uint64_t, uint8_t, uint64_t*);

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 21" WHITE "           |\n" RESET);
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

	// timing
	clock_t time_start, time_end;

	// parse variables
	uint8_t state = 0;
	uint8_t idx = 0;
	uint8_t instr_idx = 0;
	char tmp_str[15];

	// instructions and solutions
	instruction_t* instructs = malloc(sizeof(instruction_t) * NUM_INSTR);
	uint64_t* reg = calloc(sizeof(uint64_t), 6);
	uint64_t p1 = 0, p2;
	uint8_t ip_reg = 0;

	printf(YELLOW "starting...\n" RESET);

	printf(YELLOW "=> " WHITE "parsing elfcode...\n\n" RESET);

	time_start = clock();

	// parse elfcode

	while ((num_read = getline(&line, &len, fp)) != -1) {
		if (state == 1) {
			instructs[instr_idx].instr = instr_set[instr_to_idx(line)];

			for (uint8_t i = 0; i < num_read; i++) {
				switch (state) {
					case 1:
						if (line[i] == ' ') { state++; }
						break;
					case 2:
						if (line[i] == ' ') {
							state++;
							tmp_str[idx] = '\0';
							idx = 0;
							instructs[instr_idx].a = atol(tmp_str);
						} else { tmp_str[idx++] = line[i]; }
						break;
					case 3:
						if (line[i] == ' ') {
							state++;
							tmp_str[idx] = '\0';
							idx = 0;
							instructs[instr_idx].b = atol(tmp_str);
						} else { tmp_str[idx++] = line[i]; }
						break;
					case 4:
						if (line[i] == '\n') {
							state++;
							tmp_str[idx] = '\0';
							idx = 0;
							instructs[instr_idx].c = atoi(tmp_str);
						} else { tmp_str[idx++] = line[i]; }
						break;
					case 5:
						i = num_read - 1;
						break;
				}
			}

			state = 1;
			line[num_read - 1] = '\0';

			printf(RED "parsed in => " WHITE "<instruction> (a,b,c) : " BLUE "0x%lx " RESET "(" GREEN "%ld,%ld,%d" RESET ")\t[ ", (intptr_t) instructs[instr_idx].instr, instructs[instr_idx].a, instructs[instr_idx].b, instructs[instr_idx].c);
			printf(WHITE "(%d)\t%s" RESET " ]\n", instr_to_idx(line), line);
			instr_idx++;
		} else {
			ip_reg = line[4] - 0x30;
			state++;
		}
	}

	printf(GREEN "\n=>" WHITE " elfcode parsed " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (clock() - time_start) * 1000.0 / CLOCKS_PER_SEC);
	printf(BLUE "=> " WHITE "running elfcode...\n" RESET);

	clock_t elf_start = clock();

	run(instructs, reg, ip_reg, &p1, &p2);

	printf(GREEN "=>" WHITE " elfcode executed! " WHITE "(" BLUE "%f s" WHITE ")\n" RESET, (double) (clock() - elf_start) / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "reg[0] with least instructions" WHITE "\t: " CYAN "%ld\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "reg[0] with most instructions" WHITE "\t: " CYAN "%ld\n" RESET, p2);

	// free up memory
	free(instructs);
	free(reg);
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
