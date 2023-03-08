/* Program to print and play checker games.*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8      /* board size */
#define ROWS_WITH_PIECES    3      /* number of initial rows with pieces */
#define CELL_EMPTY          '.'    /* empty cell character */
#define CELL_BPIECE         'b'    /* black piece character */
#define CELL_WPIECE         'w'    /* white piece character */
#define CELL_BTOWER         'B'    /* black tower character */
#define CELL_WTOWER         'W'    /* white tower character */
#define COST_PIECE          1      /* one piece cost */
#define COST_TOWER          3      /* one tower cost */
#define TREE_DEPTH          3      /* minimax tree depth */
#define COMP_ACTIONS        10     /* number of computed actions */
#define INITIAL_B           12
#define INITIAL_W           12
#define EVEN                0      /* to check if (value)%2 is even or odd */
#define TOT_EQUAL_SYMB      37		 /* no of '=' symbols before each action */
#define FIRST_COL           'A'    /* first column heading */
#define LAST_COL            'H'    /* last column heading */
#define FIRST_ROW           1      /* first row heading */
#define LAST_ROW            8      /* last row heading */
#define FIRST_INDEX         0      /* starting index of 2D board array */
#define LAST_INDEX          7      /* last index of 2d board array */
#define MAX_MOVE            2		 /* ditance of maximum move possible */
/* used to determine row and col after normal cell move */
#define NORTH               (-1)
#define SOUTH               1
#define EAST                1     
#define WEST                (-1)
/* used to determine row and col after cell move to capture opponent cell */
#define CNORTH              (-2)
#define CSOUTH              2
#define CEAST               2
#define CWEST               (-2)
#define TOWER_MOVES         4
#define INITIAL_W_OR_B      12
/* maximum limit of decendant boards that can be reached from a single board
   configuration in a particular turn */
#define UPPER_LIMIT         TOWER_MOVES*INITIAL_W_OR_B	

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  /* board type */
/* end of initial skeleton ---------------------------------------------------*/

typedef struct {
		char s_col, t_col;	
		int  s_row, t_row;
}command_t;		/* `s` represents source and `t` represents target */
               /* stores action for a specific turn */

typedef struct node node_t;
typedef struct allboard allboard_t;

struct node {
	board_t    board;	 /* descendant board */
	command_t  command; /* stores the command, used to reach descendant board*/
	allboard_t *next;   /* pointer to all further descendants of this board */                                             
	int        cost;    /* used in cost propogation step */
};

struct allboard {
	int     nboard;			/* to keep count of descendant nodes in an array */
	node_t  *poss_boards;	/* pointer to an array of descendants nodes */
};

/* function prototypes */
void fill_board(board_t board);
void formatting(void);
void equal_line(void);
void print_board(board_t board);
int col_num(char col);
int middle_index(int x1, int x2);
void print_initial(board_t board);
void print_board(board_t board);                                   
char do_stage0(board_t board, int *turns);
int is_valid(board_t board, command_t *command, int turns);
int is_legal_action(board_t board,command_t *command, char s_cell, int turns);
void valid_capture(board_t board, int turns, int c_row, int c_col);
int board_cost(board_t board);
char col_char(int x);
int move_diff(int x1, int x2);
void do_stage1(board_t board, int turns);
allboard_t *insert_at_root(board_t board);
allboard_t *make_descendants(void);
void copy_board(board_t orig_board, board_t cpy_board);
void rec_tree(allboard_t *allboard, board_t board, int base, int turns);
allboard_t *board_moves(allboard_t *allboard, board_t board, int turns);
allboard_t *move_cell(int i, int j, int row_dir, int col_dir, int c_row_dir, 
	                  int c_col_dir,allboard_t *allboard, board_t board, 
	                  char cell, char cell_t, char opp_cell1, char opp_cell2, 
	                  int furthest_row);
allboard_t* add_descendant(allboard_t *allboard, command_t *temp_command, 
	                       board_t cpy_board);
int within_board(int t_row, int t_col);
void leaf_cost(allboard_t *allboard, node_t *leaf_node);
void cost_propogation(allboard_t *allboard, node_t *parent, int turns);
void min_cost(allboard_t *childs, node_t *parent);
void max_cost(allboard_t *child, node_t *parent);
void next_move(allboard_t *childs, int turns, board_t main_board);
void print_stage1(node_t *child, int turns, board_t main_board);
int all_null(allboard_t *allboard);
void free_tree(allboard_t *allboard);

/****************************************************************/
/* main program controls all the action
*/
int
main(int argc, char *argv[]) {                                       
    /* YOUR IMPLEMENTATION OF STAGES 0-2 */
    int turns=0, i;
    char next;
    board_t board;
    
    /* Initial board setup */
    fill_board(board);	                                                               
	print_initial(board);                                            
	next=do_stage0(board, &turns);
	/* stage 1 implementation ('A' command at the end of input file) */
	if (next == 'A') {                                              
		do_stage1(board, turns);
	  /* stage 2 implementation ('P' command at the end of input file) */
	} else if (next == 'P') {
		for (i=0; i<COMP_ACTIONS; i++) {
			do_stage1(board, turns);
			turns += 1;
		}
	}
	
    return EXIT_SUCCESS;            /* exit program with the success code */
}

/****************************************************************/  
/* fills the board array to initial setup 
*/
void
fill_board(board_t board) {                    
	int i, j;
	
	/* initial initialization of board to all '.' charachters */
	for (i=0; i<BOARD_SIZE; i++) {
		for (j=0; j<BOARD_SIZE; j++) {     
			board[i][j] = CELL_EMPTY; 
		}  
	}
	/* now board initialization to required setup */
	
	/* initialization of all odd index rows, `i` starts from odd index 1 */
	/* 1st element is cell piece so `j` starts from 0 and increments by 2 */
	for (i=1; i<BOARD_SIZE; i+=2) {
		for (j=0; j<BOARD_SIZE; j+=2) {	
			                              
			/* initialization of white rows */
			if(i<ROWS_WITH_PIECES) {
				board[i][j] = CELL_WPIECE;
			/* initialization of black rows */
			} else if (i >= (BOARD_SIZE - ROWS_WITH_PIECES)) {
					board[i][j] = CELL_BPIECE;
			}                                  
		}
	}
	/* initialization of all even index rows, `i` starts from even index 0 */
	/* cell piece is a 2nd element so `j` starts from 1 and increments by 2 */
	for (i=0; i<BOARD_SIZE; i+=2) {
		for (j=1; j<BOARD_SIZE; j+=2) {	
			                              
            /* initialization of white rows */
            if(i<ROWS_WITH_PIECES) {
					board[i][j] = CELL_WPIECE; 
				  /* initialization of black rows */
			} else if (i >= (BOARD_SIZE - ROWS_WITH_PIECES)) {
					board[i][j] = CELL_BPIECE;
			}
		}
	}
	return;
}   

/****************************************************************/
/* some simple helper functions 
*/                                                          
 
/* helps in formatting within rows of board */
void 
formatting(void) {
	int i;          
	printf("   +");                  
	for (i=0; i<BOARD_SIZE; i++) {                             
		printf("---+");                                
	}
	printf("\n");
	return;                 
}        
                                                    
/* Use to print equal charachters line before each action */
void
equal_line(void) {
	int i;
	for (i=0; i<TOT_EQUAL_SYMB; i++) {
		printf("=");
	}
	printf("\n");
	return;
}

/* takes the col number in char and returns its integer representation that 
   is from 1 to 8 inclusive */
int
col_num(char col) {
	char i;
	int num=0;
	
	for (i=FIRST_COL; i<=LAST_COL; i++) {
		num += 1;
		if (i==col) {
			return num;
		}
	}
	return 0;
}

/* used to calculate distance b/w target row/(cols) and source row/(cols) */
int
move_diff(int x1, int x2) {
	if ((x1-x2) < 0) {
		return -(x1-x2);
	}

	return (x1-x2);
}


/* gives a middle diagonal index (used to get index of captured cell) */
int 
middle_index(int x1, int x2) {
	if (x1>x2) {
		return (x1 - 1);
	} else {
		return (x2 - 1);
	}
}

/* takes a board column index and returns a `char` as a column representation
   in game play */
char
col_char(int x) {
	char c;
	int index=0;
	for (c=FIRST_COL; c<=LAST_COL; c++) {
		if (index == x) {
			break;
		}
		index += 1;
	}
	return c;
}
		
/****************************************************************/
/* print the initial setup that is a start of the game 
*/
void
print_initial(board_t board) {

	printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
	printf("#BLACK PIECES: %d\n", INITIAL_B);
	printf("#WHITE PIECES: %d\n", INITIAL_W);
		
	print_board(board);
	return;
}

/****************************************************************/
/* use to print the 2D board array with a proper board formatting 
*/
void 
print_board(board_t board) {
	char c;
	int i, j;
	printf("  ");
	/* printing column headings */
	for (c=FIRST_COL; c<=LAST_COL; c++) {
		printf("   %c", c);
	}
	printf("\n");
	
	/* printing row headings along with board values */	
	for (i=0; i<BOARD_SIZE; i++) {
		formatting();	/* formatting between rows */
		printf(" %d |", i+1);	/* row headings */
		for (j=0; j<BOARD_SIZE; j++) {
			printf(" %c |", board[i][j]);	/* board values */
		}
	printf("\n");
	}
	formatting();
	return;
}

/****************************************************************/
/* read, analyze and print the input data. prints the current board conditions
   for each command and exit the play if any provided command is invalid
*/
char
do_stage0(board_t board, int *turns) {
	int move;
	int t_row_index, t_col_index, s_row_index, s_col_index;
	int c_row, c_col;	/* row and col index of captured cell in `board` */
	command_t command;
	
	/* read the input data from the file and copy that into struct `command` */
	while ((scanf("%c%d-%c%d\n", &command.s_col, &command.s_row, 
			&command.t_col, &command.t_row)) == 4) {
	
	    /* checks the validity of input command and if valid then proceed */
		if (is_valid(board, &command, *turns)) {
			
			/* indexes to be used as a subsripts for array `board` */
			t_row_index =  command.t_row - 1;
			t_col_index = col_num(command.t_col) - 1;
			s_row_index = command.s_row - 1;
			s_col_index = col_num(command.s_col) - 1;
			
			/* target cell is set to source cell and source cell is then 
			replaced with an empty cell */
			board[t_row_index][t_col_index] = board[s_row_index][s_col_index];
			board[s_row_index][s_col_index] = CELL_EMPTY;
			
			/* check if any cell is captured and so remove that */
			move = t_row_index-s_row_index;
			if (move == -MAX_MOVE || move == MAX_MOVE) {
				c_row = middle_index(s_row_index, t_row_index);
				c_col = middle_index(s_col_index, t_col_index);
				/* check whether captured cell was valid with respect to
				   source cell, otherwise print error message */
				valid_capture(board, *turns, c_row, c_col);
				board[c_row][c_col] = CELL_EMPTY;
			}
			
			/* all fine so now proceed with printing of data */
			equal_line();
			/* Black move if turns is even */
			if ((*turns)%2 == EVEN) {
				/* black reches lowest white row so must be turned to tower */
				if (t_row_index == FIRST_INDEX) {
					board[t_row_index][t_col_index] = CELL_BTOWER;
				}
				*turns += 1;
				printf("BLACK ACTION #%d: %c%d-%c%d\n", *turns, command.s_col, 
						command.s_row, command.t_col, command.t_row);
				
			  /* turn is odd so white move */
			} else {
				/* white reaches lowest black row so must be turned to tower */
				if (t_row_index == LAST_INDEX) {
					board[t_row_index][t_col_index] = CELL_WTOWER;
				}
				*turns += 1;
				printf("WHITE ACTION #%d: %c%d-%c%d\n", *turns, command.s_col, 
						command.s_row, command.t_col, command.t_row);
			}
			printf("BOARD COST: %d\n", board_cost(board));
			print_board(board);
		}
	}
	return (command.s_col);	/* to be used for stage 1 or 2 */
}

/****************************************************************/
/* checks for the validity of move from source to target cell and if invalid
   it terminates the program with corresponding error printing 
*/
int
is_valid(board_t board, command_t *command, int turns) {
	int s_col_num, t_col_num;	/* source and target column number */
	char s_cell, t_cell;	    /* source and traget cell */
	
	
	if (command->s_col < FIRST_COL || command->s_col > LAST_COL
		|| command->s_row < FIRST_ROW || command->s_row > LAST_ROW) {
		printf("ERROR: Source cell is outside of the board.\n");
		exit(EXIT_FAILURE);
	}
	if (command->t_col < FIRST_COL || command->t_col > LAST_COL
		|| command->t_row < FIRST_ROW || command->t_row > LAST_ROW) {
		printf("ERROR: Target cell is outside of the board.\n");
		exit(EXIT_FAILURE);
	}
	
	/* doing this assignments after first two errors to avoid any garbage 
	   value */
	s_col_num = col_num(command->s_col);	/* col num without 0 offset */
	t_col_num = col_num(command->t_col);  /* col num without 0 offset */
	/* each row and col subtracted with 1 to be used as index of board */ 
	s_cell = board[command->s_row - 1][s_col_num - 1];	/* source cell */
	t_cell = board[command->t_row - 1][t_col_num - 1];	/* target cell */
	
	if (s_cell == CELL_EMPTY) {
		printf("ERROR: Source cell is empty.\n");
		exit(EXIT_FAILURE);
	} 
	if (t_cell != CELL_EMPTY) {
		printf("ERROR: Target cell is not empty.\n");
		exit(EXIT_FAILURE);
	}
	/* prints error if source contain white piece/tower provided turn was
	   even that is black move */
	if (turns%2 == EVEN && (s_cell == CELL_WPIECE || s_cell == CELL_WTOWER)) {
		printf("ERROR: Source cell holds opponent’s piece/tower.\n");
		exit(EXIT_FAILURE);
	} 
	/* prints error if source contain black piece/tower provided turn was
	   odd that is white move */
	if (turns%2 != EVEN && (s_cell == CELL_BPIECE || s_cell == CELL_BTOWER)) {
		printf("ERROR: Source cell holds opponent's piece/tower.\n");
		exit(EXIT_FAILURE);
	}
	/* check for other illegal actions */
	if (!is_legal_action(board, command, s_cell, turns)) {
		printf("ERROR: Illegal action.\n");
		exit(EXIT_FAILURE); 
	}
	
	return 1;
}

/****************************************************************/
/* returns 0 if the action is not legal, otherwise returns 1 
*/
int
is_legal_action(board_t board, command_t *command, char s_cell, int turns) { 
 	int diff_row, diff_col;  /* row and col difference b/w source and target */
 	 
 	diff_row = move_diff((command->s_row),(command->t_row));
	diff_col = move_diff(col_num(command->s_col),col_num(command->t_col));
	 
	/* row or col differnce b/w target and source is more than 2 */
	if (diff_row > MAX_MOVE || diff_col > MAX_MOVE) {
		return 0;
	}
	/* moving within same row or columns (error)*/
	if ((command->t_row == command->s_row) 
	 	|| (command->t_col == command->s_col)) {
		return 0;
	}
 	/* black piece move if even turn */
	if (turns%2 == EVEN && s_cell==CELL_BPIECE) {
		/* black piece moving in wrong SOUTH direction */
 	 	if ((command->t_row)>(command->s_row)) {
 	 	 	return 0;
 	 	}
 	}
	/* white piece move if odd turn */
 	if (turns%2 == EVEN && s_cell==CELL_WPIECE) {	
 		/* white piece moving in wrong NORTH direction */
 		if ((command->t_row)<(command->s_row)) {
 			return 0;
 		}
 	}
 	return 1;
}

/****************************************************************/
/* exit the program with error message if capture was not valid that is 
   capture cell is not a opponent cell or it was empty */
void 
valid_capture(board_t board, int turns, int c_row, int c_col) {
	int c_cell = board[c_row][c_col];	/* captured cell */ 
	
	if (turns%2 == EVEN){		/* blacks have captured a cell (even turn)*/
		if (c_cell == CELL_BPIECE || c_cell == CELL_BTOWER 
			|| c_cell == CELL_EMPTY) {
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE);
		}
	} else if (turns%2 != EVEN) {		/* white move (odd turn) */
		if (c_cell == CELL_WPIECE || c_cell == CELL_WTOWER 
			|| c_cell == CELL_EMPTY) {
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE);
		}
	}
	return;
}
/****************************************************************/
/* calculates and returns the board cost using the formula (b + 3B − w − 3W)
*/
int
board_cost (board_t board) {
	int i=0, j=0;
	int b_pieces=0, b_towers=0, w_pieces=0, w_towers=0;
	char cell;

	/* traverse the `board` in row major order and count the number of each 
	   cell type */
	for (i=0; i<BOARD_SIZE; i++) {
		for (j=0; j<BOARD_SIZE; j++) {
			cell = board[i][j];
			if (cell == CELL_BPIECE) {
				b_pieces += 1;
			} else if (cell == CELL_BTOWER) {
				b_towers += 1;
			} else if (cell == CELL_WPIECE) {
				w_pieces += 1;
			} else if (cell == CELL_WTOWER) {
				w_towers += 1;
			}
		}
	}
	return (b_pieces + b_towers*COST_TOWER - w_pieces - w_towers*COST_TOWER);
}

/****************************************************************/
/* stage-1 compute and print next action 
*/
void
do_stage1(board_t board, int turns) {
	allboard_t *root;
	root = insert_at_root(board);
	/* tree construction of descendant board configurations upto a required 
	   depth (TREE_DEPTH) */
	rec_tree(root, board, TREE_DEPTH, turns);
	/* no possible board descendants so decide a winner */
	if (root->poss_boards[0].next == NULL) {
		if (turns%2 == EVEN) {
			printf("WHITE WIN!\n");
			exit(EXIT_SUCCESS);
		} else if (turns%2 != EVEN) {
			printf("BLACK WIN!\n");
			exit(EXIT_SUCCESS);
		}
	}
	/* stage 1 (part 2) cost calculation of boards at leaf nodes */
	leaf_cost(root, &(root->poss_boards[0]));
	/* stage 1 (part 3) cost propogation to parent node using minimax 
	   algorithm */
	cost_propogation(root, &(root->poss_boards[0]), turns);
	/* next action according to the propogated cost */
	next_move(root->poss_boards[0].next, turns, board);
	free_tree(root);	/* All done, so, free the tree memory */
	root = NULL;
	return;
}
	
/****************************************************************/
/* insertion of recent board from stage 0 and returns a pointer to it
*/
allboard_t
*insert_at_root(board_t board) {
	allboard_t *root;
	node_t *new;
	
	root = make_descendants();
	assert(root!=NULL);
	/* malloc the space of a struct node_t to store the 2D board array */
	root->poss_boards = (node_t*)malloc(sizeof(*new)); 
	assert(root->poss_boards!=NULL);
	/* storing board in node_t */
	copy_board(board, root->poss_boards->board);
	root->nboard = 1;	/* array size */
	
	return root;
}
			
/****************************************************************/
/* makes a descendants struct of type allboard_t
*/
allboard_t
*make_descendants(void) {
	allboard_t *tree;
	
	tree = (allboard_t*)malloc(sizeof(*tree));
	assert(tree!=NULL);
	tree->poss_boards = NULL;
	tree->nboard = 0;
	
	return tree;
}
/****************************************************************/
/* a helper function to copy the board configuration of parent board which can
   can be further modified during stage 1 to make a descendant board
   */
void
copy_board(board_t orig_board, board_t cpy_board) {
	int i,j;
	
	for (i=0; i<BOARD_SIZE; i++) {
		for (j=0; j<BOARD_SIZE; j++) {
			cpy_board[i][j] = orig_board[i][j];
		}
	}
	return;
}
/****************************************************************/
/* recursively makes a descendant tree of a recent board configuration from
   stage 0 upto a required depth `base`, `board` is a parent board, `turns`
   used to keep count of next action of either black or white 
*/
void
rec_tree(allboard_t *allboard, board_t board, int base, int turns) {
	int i;
	allboard_t *next;
	
	if (base<1) {
		allboard = NULL;	/* lowest depth reached */
		return;
	}
	assert(allboard!=NULL);
	
	/* iterate through all nodes at a particular level and construct the
	   descendants of each board from a node if possible */
	for (i=0; i<(allboard->nboard); i++) {
		allboard->poss_boards[i].next = make_descendants();
		assert(allboard->poss_boards[i].next!=NULL);
		
		/* descendant nodes construction from a parent board */
		next = board_moves(allboard->poss_boards[i].next, 
			               allboard->poss_boards[i].board, turns);
		assert(next!=NULL);
		/* assignmnet of handle for descendants to a pointer in parent node */
		allboard->poss_boards[i].next = next;

		/* no possible actions/descendants can be made from parent board */
		if (allboard->poss_boards[i].next->nboard == 0) {
			
			if (turns%2 == EVEN) {		/* black turns no descendant made */
				allboard->poss_boards[i].cost = INT_MIN;
			} else {	/* white turn no descendant found */
				allboard->poss_boards[i].cost = INT_MAX;
			}
			/* also free the struct type allboard_t that was made to store 
			descendants nodes for this board */
			free(allboard->poss_boards[i].next);
			allboard->poss_boards[i].next = NULL;
		}
		
		/* recursive call for that parent board only that has descendants */
		if (allboard->poss_boards[i].next != NULL) { 
			rec_tree(allboard->poss_boards[i].next, 
			   		 allboard->poss_boards[i].board, base-1, turns+1);
		}
	}
}
/****************************************************************/
/* takes the the parent board `board` and the pointer from parent node 
   `allboard` it explores all the possible actions of parent board to 
   construct descendants using a helper function `move_cell`. It provides all
   possible directions to `move_cell` starting from NORTH-EAST and moving
   clockwise for a particular piece or tower(encountered while traversing 
   parent board in row major order) 
*/
allboard_t
*board_moves(allboard_t *allboard, board_t board, int turns) {
	int i, j;
	assert(allboard!=NULL);
	
	for (i=0; i<BOARD_SIZE; i++) {
		for (j=0; j<BOARD_SIZE; j++) {
			
			/* black move,so explore actions for black pieces and tower only*/
			if (turns%2 == EVEN) {	
			    /* black piece(NORTH-EAST or NORTH-WEST) */
				if (board[i][j]==CELL_BPIECE){
					allboard = move_cell(i, j, NORTH, EAST, CNORTH, CEAST, 
					                 	  allboard, board, CELL_BPIECE, 
					                 	  CELL_BTOWER, CELL_WPIECE, 
					                 	  CELL_WTOWER, FIRST_INDEX);
					
					allboard = move_cell(i, j, NORTH, WEST, CNORTH, CWEST, 
					                     allboard, board, CELL_BPIECE, 
					                     CELL_BTOWER, CELL_WPIECE, 
					                     CELL_WTOWER, FIRST_INDEX);
				  /* explore all possible directions for black tower
				     (NORTH-EAST, SOUTH-EAST, SOUTH-WEST, NORTH-WEST) */
				} else if (board[i][j] == CELL_BTOWER) {
					allboard = move_cell(i, j, NORTH, EAST, CNORTH, CEAST, 
					                     allboard, board, CELL_BTOWER, 
					                     CELL_BTOWER, CELL_WPIECE, 
					                     CELL_WTOWER, FIRST_INDEX);
					
					allboard = move_cell(i, j, SOUTH, EAST, CSOUTH, CEAST, 
					                     allboard, board, CELL_BTOWER, 
					                     CELL_BTOWER, CELL_WPIECE, 
					                     CELL_WTOWER, FIRST_INDEX);
					
					allboard = move_cell(i, j, SOUTH, WEST, CSOUTH, CWEST, 
					                     allboard, board, CELL_BTOWER, 
					                     CELL_BTOWER, CELL_WPIECE, 
					                     CELL_WTOWER, FIRST_INDEX);
					
					allboard = move_cell(i, j, NORTH, WEST, CNORTH, CWEST, 
					                     allboard, board, CELL_BTOWER, 
					                     CELL_BTOWER, CELL_WPIECE, 
					                     CELL_WTOWER, FIRST_INDEX);
				}
			}
			/* white move so explore actions for white pieces and tower only */
			if (turns%2 != EVEN) {	
				/* 	white piece (SOUTH EAST, SOUTH-WEST) */
				if (board[i][j] == CELL_WPIECE) {
					allboard = move_cell(i, j, SOUTH, EAST, CSOUTH, CEAST, 
					                     allboard, board, CELL_WPIECE, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);
					
					allboard = move_cell(i, j, SOUTH, WEST, CSOUTH, CWEST, 
									      allboard, board, CELL_WPIECE, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);
					
				  /* white tower(directions same for black tower) */	
				} else if (board[i][j] == CELL_WTOWER) {
					allboard = move_cell(i, j, NORTH, EAST, CNORTH, CEAST, 
					                  	  allboard, board, CELL_WTOWER, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);
					
					allboard = move_cell(i, j, SOUTH, EAST, CSOUTH, CEAST, 
					                     allboard, board, CELL_WTOWER, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);
					
					allboard = move_cell(i, j, SOUTH, WEST, CSOUTH, CWEST, 
					                     allboard, board, CELL_WTOWER, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);
					
					allboard = move_cell(i, j, NORTH, WEST, CNORTH, CWEST, 
						                 allboard, board, CELL_WTOWER, 
					                     CELL_WTOWER, CELL_BPIECE, 
					                     CELL_BTOWER, LAST_INDEX);

				}
			}
		}
	}
	assert(allboard!=NULL);
	return allboard;
}
			
/****************************************************************/
/* a general function that makes a descendent boards for each valid moves
   using a helper function `add_descendant`. `cell_t` is a tower of choosen
   cell, `i` is a row of chosen cell, `j` is a column of chosen cell, 
   `row_dir` & `col_dir` is a direction towards which move is considered, 
   `c_row_dir` & `c_col_dir` is a direction in which a `cell` can move to 
   capture any of opponent cell either piece or tower represented by 
   `opp_cell1` & `opp_cell2`,`furthest row` is a lowest opponent row at 
   which `cell` can reach to convert into a tower.
*/
allboard_t
*move_cell(int i, int j, int row_dir, int col_dir,int c_row_dir,int c_col_dir,
	       allboard_t *allboard, board_t board, char cell, char cell_t, 
	       char opp_cell1, char opp_cell2, int furthest_row) {
	int move_occured=0;
	board_t cpy_board;
	command_t temp_command;
	
	assert(allboard!=NULL);
	copy_board(board, cpy_board);
	
	/* check if diagonal move is within the board to proceed further */
	if (within_board(i+row_dir, j+col_dir)) {
		
			/* check if diagonal is empty and so make a move to it */		
		if (board[i + row_dir][j + col_dir] == CELL_EMPTY) {
			cpy_board[i + row_dir][j + col_dir] = cell;
			cpy_board[i][j] = CELL_EMPTY;
			move_occured = 1;
			/* reached lowest opposite row change to tower */
			if (i+row_dir == furthest_row) {
				cpy_board[i + row_dir][j + col_dir] = cell_t;
			}
			/* +1 added in rows index so command represent row headings 
			   according to game configuration and `col_char` directly gives
			   column heading taking a board index */
			temp_command.s_row = i + 1; 
			temp_command.s_col = col_char(j);
			temp_command.t_row = (i + row_dir + 1); 
			temp_command.t_col = col_char(j + col_dir);
						
		  /* check for capturing a cell if diagonal contains an opponent cell*/
		} else if((board[i + row_dir][j + col_dir] == opp_cell1) 
				  || board[i + row_dir][j + col_dir] == opp_cell2) {
		        
			/* check if capturing move is within board limit */
			if (within_board(i+c_row_dir, j+c_col_dir)	
				&& cpy_board[i + c_row_dir][j + c_col_dir] == CELL_EMPTY) {
				/* make a move to capture */	    
				cpy_board[i+c_row_dir][j+c_col_dir] = cell;
				/* remove the captured cell and empty the source cell as well*/
				cpy_board[i+row_dir][j+col_dir] = CELL_EMPTY;
				cpy_board[i][j] = CELL_EMPTY;
				move_occured = 1;
				/* cell reached lowest opposite row change to tower */
				if (i+c_row_dir == furthest_row) {
						cpy_board[i+c_row_dir][j+c_col_dir] = cell_t;
				}
			   /* +1 added so command represent row representations in terms
			   of board game configuration */
			   temp_command.s_row = i+1; 
			   temp_command.s_col = col_char(j);
			   temp_command.t_row = (i+c_row_dir+1); 
			   temp_command.t_col = col_char(j+c_col_dir);
			}
	   }
   }
   if (move_occured) {
   	   return (add_descendant(allboard, &temp_command, cpy_board));
   }
   return allboard;
}

/****************************************************************/
/* used to verify whether the move is within the board, It takes the argument
   of type index of 2D board array 
*/
int
within_board(int t_row, int t_col) {
	
	if (t_row < FIRST_INDEX || t_row > LAST_INDEX) { 
		return 0;
	}
	if (t_col < FIRST_INDEX || t_col > LAST_INDEX) {
		return 0;
	}
	/* must be within the board so return 1 */
	return 1;
}

/****************************************************************/
/* it builds on the the array of descendant boards */
allboard_t*
add_descendant(allboard_t *allboard, command_t *temp_command, 
	           board_t cpy_board) {
	int n;
	node_t *new;
	
	assert(allboard!=NULL);
	/* first descendant to be added so request the memory for array of
	descendant/child  nodes */
	if (allboard->nboard == 0) {
		n = allboard->nboard;
		allboard->poss_boards = (node_t*)malloc(UPPER_LIMIT*sizeof(*new));
		assert(allboard->poss_boards!=NULL);
		/* copying the descendant board and command(used to reach this board
		   configuration) into a descendant node */
		copy_board(cpy_board, allboard->poss_boards[n].board);
		allboard->poss_boards[n].next = NULL;
		allboard->poss_boards[n].command = *temp_command;
		allboard->nboard += 1;		/* size of descendant array */
				
	/* appending an array of descendants with a new descendant */
  	} else {
  		n = allboard->nboard;
		copy_board(cpy_board, allboard->poss_boards[n].board);
		allboard->poss_boards[n].next = NULL;
		allboard->poss_boards[n].command = *temp_command;
		allboard->nboard += 1;	   /* increment the size of descendant array */
	}
	return allboard;	
}

/****************************************************************/
/* calculates the cost of the boards in leaf nodes recursively
*/
void
leaf_cost(allboard_t *allboard, node_t *leaf_node) {
	int i;
	
	if (allboard==NULL) {
		/* reached to lowest level node in which it pointer points to no
		   further descendants. so calculate the cost of board in that node */ 
		assert(leaf_node!=NULL);
		leaf_node->cost = board_cost(leaf_node->board);
		return;
	}
	assert(allboard!=NULL);
	/* iterate through the array of descendants nodes and call a recursive
	   call on each node */
	for (i=0; i<(allboard->nboard); i++) {
		leaf_cost(allboard->poss_boards[i].next, &(allboard->poss_boards[i]));
		
	}
}
	
/****************************************************************/
/* propagates the cost up the tree according to min-max rule by calling the
   corresponding functions `min_cost` and `max_cost` decided by `turns`
*/
void
cost_propogation(allboard_t *allboard, node_t *parent, int turns) {
	int i;
	
	if (allboard==NULL) {
		return;
	}
	assert(allboard!=NULL && parent!=NULL);
	
	/* iterate through the array of descendants nodes and call a recursive
	   call on each node */
	for (i=0; i<(allboard->nboard); i++) {
		
		/* allboard->poss_boards[i] this is parent node */
		cost_propogation(allboard->poss_boards[i].next, 
			 			  &(allboard->poss_boards[i]), turns+1);
		
		/* turn at descendant level is even so parent must be white and so 
		   chose the lowest cost from child nodes */
		if (turns%2 == EVEN) {
			min_cost(allboard, parent);
		/* turn at descendant level is odd so parent must be black and so 
		   chose the max cost from child nodes */
		} else if (turns%2 != EVEN) {
			max_cost(allboard, parent);
	    }
	}
}

/****************************************************************/
/* Takes the pointer to descendants pointed from parent node. Iterate through
   the child/descendant boards and selects the min cost and assign to `parent`
   node
*/
void
min_cost(allboard_t *childs, node_t *parent) {
	int i, min;
	assert(childs!=NULL && parent!=NULL);
	
	min = childs->poss_boards[0].cost;
	for (i=1; i<(childs->nboard); i++) {
		if (min > childs->poss_boards[i].cost) {
			min = childs->poss_boards[i].cost;
		}
	}
	parent->cost = min;
	return;
}
			
/****************************************************************/
/* Takes the pointer to descendants pointed from parent node. Iterate through
   the child/descendant boards and selects the max cost and assign to `parent`
   node
*/
void
max_cost(allboard_t *childs, node_t *parent) {
	int i, max;
	assert(childs!=NULL && parent!=NULL);
	
	max = childs->poss_boards[0].cost;
	for (i=1; i<(childs->nboard); i++) {
		if (max < childs->poss_boards[i].cost) {
			max = childs->poss_boards[i].cost;
		}
	}
	parent->cost = max;
	return;
}
	
/****************************************************************/
/* on the basis of turns and minmax decision rule it selects the most 
   favourable child node. `main_board` is a main 2d array of board_t from
   main function that keep record of current board status in a game */
void
next_move(allboard_t *childs, int turns, board_t main_board) {
	int i, board_index=0, cost;
	
	/* black action. select the index of a child node with higest score */
	if (turns%2 == EVEN) {
		cost = childs->poss_boards[0].cost;
		for (i=1; i<(childs->nboard); i++) {
			if (cost < childs->poss_boards[i].cost) {
				cost = childs->poss_boards[i].cost;
				board_index = i;
			}
		}
		
		print_stage1(&(childs->poss_boards[board_index]), turns, main_board);
	
     /* white action. select the index of a child node with lowest score */
	} else {
		cost = childs->poss_boards[0].cost;
		for (i=1; i<(childs->nboard); i++) {
			if (cost > childs->poss_boards[i].cost) {
				cost = childs->poss_boards[i].cost;
				board_index = i;
			}
		}
		print_stage1(&(childs->poss_boards[board_index]), turns, main_board);
		
	}
	return;
}

/****************************************************************/
/* prints the rquired output of stage 1 from a selected child node and 
   edits the main board_t array with the new board status after stage 1
*/
void
print_stage1(node_t *child, int turns, board_t main_board) {
	command_t action;
	action = child->command;
	
	equal_line();
	printf("*** ");
	
	/* Even turn black action */
	if (turns%2 == EVEN) {
		printf("BLACK ACTION #%d: %c%d-%c%d\n", turns+1, action.s_col, 
			   action.s_row, action.t_col, action.t_row);
		printf("BOARD COST: %d\n", board_cost(child->board));
		print_board(child->board);
	
	/* odd turn white action */
	} else if (turns%2 != EVEN) {
		printf("WHITE ACTION #%d: %c%d-%c%d\n", turns+1, action.s_col, 
			   action.s_row, action.t_col, action.t_row);
		printf("BOARD COST: %d\n", board_cost(child->board));
		print_board(child->board);
	}
	/* action completed edit the main_board(board array from main function) */
	copy_board(child->board, main_board);
	return;
}
	
/****************************************************************/
/* All done free the memory occupied by the tree 
*/
void
free_tree(allboard_t *allboard) {
	int i;
	
	if (allboard==NULL) {
		return;
	}
	assert(allboard!=NULL);
	
	/* recursively iterate through the tree and free the array of nodes and
	   then free the memory (allboard_t) that contains a pointer to that array 
	   and the buddy variable at each level */
	for (i=0; (allboard!=NULL && i<(allboard->nboard)); i++) {
		free_tree(allboard->poss_boards[i].next);
		if (all_null(allboard)) {
			free(allboard->poss_boards); 
			allboard->poss_boards = NULL;
			free(allboard); 
			allboard = NULL;
			break;
		}
	}
}
	
/****************************************************************/
/* returns 0 if any of the node in an array have futher descendants, 
   otherwise returns 1
*/
int
all_null(allboard_t *allboard) {
	int i;
	
	for (i=0; i<(allboard->nboard); i++) {
		if (allboard->poss_boards[i].next != NULL) {
			return 0;
		}
	}
	return 1;
}

/****************************************************************/
                     /* "algorithms are fun" */
