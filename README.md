# Checkers
An AI checkers game
Used minimax decision rule for the tree of depth three to analyze next move
To run the game use command:
./checkers.c < test.txt

Note that to provide implementaion of game I have given test.txt file. One can generate thier own test files as they wish :).
description about test file:
When the ‘P’ command follows the input actions, your program should Play ten next actions or all actions until the end of the game, whatever comes first.

Checkers, or draughts, is a strategy board game played by two players. There are many variants of checkers. For a guide to checker’s families and rules, see https://www.fmjd.org/downloads/Checkers_families_ and_rules.pdf

Setup: An 8x8 chessboard with 12 black and 12 white pieces initially positioned

Gameplay: Each player plays all pieces of the same color. Black open the game by making a move, then white make a move, and then players alternate their turns. In a single turn, the player either makes a move or capture.

Moves: A piece may move to an empty cell diagonally forward (toward the opponent; north for black and south for white) one square.

Towers: When a piece reaches the furthest row (the top row for black and the bottom row for white), it becomes a tower (a pile of pieces). A tower may move to an empty cell diagonally, both forward and backward, one square.

Captures: To capture a piece or a tower of the opponent, a piece or a tower of the player jumps over it and lands in a straight diagonal line on the other side. This landing cell must be empty. When a piece or tower is captured, it is removed from the board. Only one piece or tower may be captured in a single jump, and, in our variant of the game, only one jump is allowed in a single turn. Hence, if another capture is available after the first jump, it cannot be taken in this turn. Also, in our variant of the game, if a player can make a move or a capture, they may decide which of the two to complete. A piece always jumps forward (toward the opponent), while a tower can jump forward and backward.

Game end: A player wins the game if it is the opponent’s turn and they cannot take action, move or capture, either because no their pieces and towers are left on the board or because no legal move or capture is possible.

