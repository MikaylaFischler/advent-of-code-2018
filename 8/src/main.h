//! @brief console colors
#define RESET	"\x1B[0m"
#define BLACK	"\x1B[0;30m"
#define RED		"\x1B[0;31m"
#define YELLOW	"\x1B[0;33m"
#define GREEN	"\x1B[0;32m"
#define BLUE	"\x1B[0;34m"
#define MAGENTA	"\x1B[0;35m"
#define CYAN	"\x1B[0;36m"
#define WHITE	"\x1B[0;37m"

#define RESET_BG	"\x1B[49m"
#define BLACK_BG	"\x1B[40m"
#define RED_BG		"\x1B[41m"
#define YELLOW_BG	"\x1B[43m"
#define GREEN_BG	"\x1B[42m"
#define BLUE_BG		"\x1B[44m"
#define MAGENTA_BG	"\x1B[45m"
#define CYAN_BG		"\x1B[46m"
#define WHITE_BG	"\x1B[47m"

#define B_BLACK		"\x1B[1;30m"
#define B_RED		"\x1B[1;31m"
#define B_YELLOW	"\x1B[1;33m"
#define B_GREEN		"\x1B[1;32m"
#define B_BLUE		"\x1B[1;34m"
#define B_MAGENTA	"\x1B[1;35m"
#define B_CYAN		"\x1B[1;36m"
#define B_WHITE		"\x1B[1;37m"

//! @brief the node struct
typedef struct node {
	uint8_t num_children;
	uint8_t num_metadata;
	uint8_t* metadata;
	struct node* children;
} node_t;

//! @brief function prototypes
void populate_tree(FILE* fp, node_t* root);
node_t* get_children(FILE* fp, node_t* root, uint8_t num_children);
uint8_t* get_metadata(FILE* fp, node_t* root, uint8_t num_metadata);
uint32_t calc_node_value(node_t* root);
void free_tree(node_t* root);
