#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>

#include "main.h"

typedef struct node {
	uint8_t num_children;
	uint8_t num_metadata;
	uint8_t* metadata;
	struct node* children;
} node_t;

uint32_t meta_data_total = 0;

void populate_tree(FILE* fp, node_t* root);
node_t* get_children(FILE* fp, node_t* root, uint8_t num_children);
uint8_t* get_metadata(FILE* fp, node_t* root, uint8_t num_metadata);

node_t* get_children(FILE* fp, node_t* root, uint8_t num_children) {
	if (num_children == 0) { return NULL; }
	node_t* children = malloc(sizeof(node_t) * num_children);

	for (int i = 0; i < num_children; i++) {
		populate_tree(fp, &children[i]);
	}

	return children;
}

uint8_t* get_metadata(FILE* fp, node_t* root, uint8_t num_metadata) {
	if (num_metadata == 0) { return NULL; }
	uint8_t* metadata = malloc(sizeof(uint8_t) * num_metadata);

	char c;
	char in_str[3];
	uint8_t idx = 0;

	for (int i = 0; i < num_metadata; i++) {
		while ((c = fgetc(fp)) != EOF) {
			if (!isspace(c)) {
				in_str[idx++] = c;
			} else {
				in_str[idx] = '\0';
				idx = 0;
				break;
			}
		}
		metadata[i] = atoi(in_str);
		meta_data_total += metadata[i];
		idx = 0;
	}

	return metadata;
}

void populate_tree(FILE* fp, node_t* root) {
	char children_str[3];
	char metadata_str[3];
	uint8_t idx = 0;
	char c;

	while ((c = fgetc(fp)) != EOF) {
		if (!isspace(c)) {
			children_str[idx++] = c;
		} else {
			children_str[idx] = '\0';
			idx = 0;
			break;
		}
	}

	while ((c = fgetc(fp)) != EOF) {
		if (!isspace(c)) {
			metadata_str[idx++] = c;
		} else {
			metadata_str[idx] = '\0';
			break;
		}
	}

	root->num_children = atoi(children_str);
	root->num_metadata = atoi(metadata_str);
	root->children = get_children(fp, root, root->num_children);
	root->metadata = get_metadata(fp, root, root->num_metadata);
}

uint32_t calc_node_value(node_t* root) {
	uint32_t sum = 0;
	if (root->num_children == 0) {
		for (int i = 0; i < root->num_metadata; i++) {
			sum += root->metadata[i];
		}
	} else {
		for (int i = 0; i < root->num_metadata; i++) {
			if (root->metadata[i] != 0 && (root->metadata[i] - 1 < root->num_children)) {
				sum += calc_node_value(&(root->children[root->metadata[i] - 1]));
			}
		}
	}

	return sum;
}

void free_tree(node_t* root) {
	if (root) {
		free(root->metadata);
		for (int i = 0; i < root->num_children; i++) { free_tree(&root->children[i]); }
		free(root->children);
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x-----------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2018 " B_BLUE "advent of code" BLUE " day 8" WHITE "           |\n" RESET);
    printf(WHITE "x-----------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end;

	node_t* root = malloc(sizeof(node_t));

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	populate_tree(fp, root);
	uint32_t root_value = calc_node_value(root);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "metadata total" WHITE "\t: " CYAN "%d\n" RESET, meta_data_total);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "value of root" WHITE "\t: " CYAN "%d\n" RESET, root_value);

	// free up memory
	fclose(fp);
	free_tree(root);
	free(root);

	return 0;
}
