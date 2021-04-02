

#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef vector<int> Board;

#define CHECK_SAME_COLOR(c1, c2)  (((c1)  < 0 && (c2) < 0) || ((c1)  > 0 && (c2) > 0 && (c1) != BEDROCK && (c2) != BEDROCK))
#define MOVE_SCORE(move, board) (piece_values[move.value] - piece_values[board[move.start]] - piece_values[board[move.end]])


const int BOARD_SIZE = 12;
int MAX_DEPTH = 3;

const int EMPTY = 0;
const int KING = 1;
const int QUEEN = 2;
const int ROOK = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int PAWN = 6;
const int BEDROCK = 7;

const int WHITE = 1;
const int BLACK = -1;

struct One_move {
	int start;
	int end;
	int value;
};


vector<int> non_bedrock_cells;
vector<int> white_promote{ QUEEN, ROOK, BISHOP, KNIGHT };
vector<int> black_promote{ -QUEEN, -ROOK, -BISHOP, -KNIGHT };

vector<int> position_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 2, 4, 4, 4, 4, 2, 1, 0, 0,
	0, 0, 1, 2, 4, 8, 8, 4, 2, 1, 0, 0,
	0, 0, 1, 2, 4, 8, 8, 4, 2, 1, 0, 0,
	0, 0, 1, 2, 4, 4, 4, 4, 2, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

vector<int> white_pawn_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 120, 120, 120, 120, 120, 120, 120, 120, 0, 0,
	0, 0, 60, 60, 60, 60, 60, 60, 60, 60, 0, 0,
	0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

vector<int> black_pawn_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, -2, -2, -2, -2, -2, -2, -2, -2, 0, 0,
	0, 0, -4, -4, -4, -4, -4, -4, -4, -4, 0, 0,
	0, 0, -60, -60, -60, -60, -60, -60, -60, -60, 0, 0,
	0, 0, -120, -120, -120, -120, -120, -120, -120, -120, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



vector<int> knigh_bishop_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


bool operator ==(const One_move& om1, const One_move& om2) {
	return (om1.start == om2.start && om1.end == om2.end && om1.value == om2.value);
}

unordered_map<char, int> promote{
	{'Q', QUEEN},
	{'R', ROOK},
	{'B', BISHOP},
	{'N', KNIGHT}
};

unordered_map<int, vector<int>> tmp_move{
	{KING, {-1, 1, BOARD_SIZE, -BOARD_SIZE, BOARD_SIZE - 1, BOARD_SIZE + 1, -BOARD_SIZE - 1, -BOARD_SIZE + 1}},
	{ROOK, {-1, 1, BOARD_SIZE, -BOARD_SIZE}},
	{BISHOP, {-BOARD_SIZE - 1, -BOARD_SIZE + 1, BOARD_SIZE + 1, BOARD_SIZE - 1}},
	{KNIGHT, {-2 * BOARD_SIZE + 1, -2 * BOARD_SIZE - 1, -BOARD_SIZE - 2, -BOARD_SIZE + 2, 2 * BOARD_SIZE + 1, 2 * BOARD_SIZE - 1, BOARD_SIZE - 2, BOARD_SIZE + 2}}
};
unordered_map<int, vector<int>> piece_move{
	{KING, tmp_move[KING]},
	{QUEEN, tmp_move[KING]},
	{ROOK, tmp_move[ROOK]},
	{BISHOP, tmp_move[BISHOP]},
	{KNIGHT, tmp_move[KNIGHT]},
	{PAWN, {-BOARD_SIZE }},
	{-KING, tmp_move[KING]},
	{-QUEEN, tmp_move[KING]},
	{-ROOK, tmp_move[ROOK]},
	{-BISHOP, tmp_move[BISHOP]},
	{-KNIGHT, tmp_move[KNIGHT]},
	{-PAWN, { BOARD_SIZE }},
	{EMPTY, {}},
	{BEDROCK, {}}
};

unordered_map<int, int> piece_values{
	{EMPTY, 0},
	{BEDROCK, 0},
	{KING, 1000},
	{QUEEN, 9},
	{KNIGHT, 3},
	{BISHOP, 3},
	{PAWN, 1},
	{ROOK, 5},
	{-KING, -1000},
	{-QUEEN, -9},
	{-KNIGHT, -3},
	{-BISHOP, -3},
	{-PAWN, -1},
	{-ROOK, -5},
};
unordered_map<int, string> piece_to_print{
	{KING, "wK"},
	{QUEEN, "wQ"},
	{ROOK, "wR"},
	{BISHOP, "wB"},
	{KNIGHT, "wN"},
	{PAWN, "wP"},
	{-KING, "bK"},
	{-QUEEN, "bQ"},
	{-ROOK, "bR"},
	{-BISHOP, "bB"},
	{-KNIGHT, "bN"},
	{-PAWN, "bP"},
	{EMPTY, "  "}
};
unordered_map<int, bool> repeated_piece{
	{KING, false},
	{QUEEN, true},
	{ROOK, true},
	{BISHOP, true},
	{KNIGHT, false},
	{-KING, false},
	{-QUEEN, true},
	{-ROOK, true},
	{-BISHOP, true},
	{-KNIGHT, false}
};

unordered_map<int, int> position_piece_score{
	{ROOK, 1},
	{BISHOP, 2},
	{KNIGHT, 2},
	{PAWN, 4},
	{QUEEN, 0},
	{KING, -1},
	{-ROOK, -1},
	{-BISHOP, -2},
	{-KNIGHT, -2},
	{-PAWN, -4},
	{-QUEEN, 0},
	{-KING, 1},
	{EMPTY, 0}
};



unordered_map<size_t, int>  board_scores;
int ncount = 0;


int find_best_score_for_black(Board& board, int depth, int best_white_score, int best_black_score);
int find_best_score_for_white(Board& board, int depth, int best_white_score, int best_black_score);
vector<One_move> get_all_moves(const Board& board, int color, vector<One_move>& output);
//Board load();

int get_score(const Board& board);


Board create_initial_board() {
	return {
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,-PAWN, -PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,PAWN, PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK
	};
}

// print the board to console text 
void print(const Board& board) {
	cout << "\n\n";
	int count = 8;
	for (int row = 2; row <= 9; row++) {
		cout << "  +--+--+--+--+--+--+--+--+\n";
		cout << count-- << " ";
		for (int col = 2; col <= 9; col++) {
			cout << "|" << piece_to_print[board[row * BOARD_SIZE + col]];
		}
		cout << "|\n";
	}
	cout << "  +--+--+--+--+--+--+--+--+\n   ";
	for (char ch = 'a'; ch < 'h' + 1; ch++) {
		cout << ch << "  ";
	}
	cout << "\n";
	cout << "score = " << get_score(board) << "\n\n";

}

// finds all non-bed-rock cells.
vector<int> find_non_bedrock() {
	vector<int> output;
	Board tmp = create_initial_board();
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i] != BEDROCK) {
			output.push_back(i);
		}
	}
	return output;
}


// Returns a hash value for a board. 
std::size_t get_hash_value(std::vector<int> const& vec) {
	std::size_t seed = vec.size();
	for (auto& i : vec) {
		seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

// Calculates the score for a board
int get_score(const Board& board) {
	ncount++;
	size_t hash_value = get_hash_value(board);

	if (board_scores.find(hash_value) != board_scores.end()) {
		return board_scores[hash_value];
	}

	// calculates the piece scores
	int piece_score = 0;
	for (int pos : non_bedrock_cells) {
		piece_score += piece_values[board[pos]];
	}

	// calculates the position scores
	int board_position_score = 0;
	for (int pos : non_bedrock_cells) {
		if (board[pos] != BEDROCK && board[pos] != EMPTY) {
			board_position_score += (board[pos] > 0)
				? position_score[pos]
				: -position_score[pos];
		}
	}

	// calculates the white pawn position scores
	for (int pos : non_bedrock_cells) {
		if (board[pos] == PAWN) {
			board_position_score += white_pawn_pos_score[pos];
		}
	}

	// calculates the black pawn position scores
	for (int pos : non_bedrock_cells) {
		if (board[pos] == -PAWN) {
			board_position_score += black_pawn_pos_score[pos];
		}
	}

	// calculates the night and bishop position scores
	for (int pos : non_bedrock_cells) {
		if (board[pos] == KNIGHT ||
			board[pos] == BISHOP ||
			board[pos] == -KNIGHT ||
			board[pos] == -BISHOP) {
			board_position_score += (board[pos] > 0)
				? knigh_bishop_pos_score[pos]
				: -knigh_bishop_pos_score[pos];
		}
	}

	if (board_scores.size() > 50000000) {
		board_scores.clear();
	}
	// merges piece score with position score.
	int score = piece_score * 10000 + board_position_score;
	board_scores[hash_value] = score;

	return score;
}

// checks if there are two kings in the board.
bool finish(Board& board) {
	int count = 0;
	for (int pos : non_bedrock_cells) {
		if (board[pos] == KING || board[pos] == -KING) {
			count++;
		}
	}
	return count != 2;
}


// find possible moves for non-pawn pieces at location pos
vector<int> move_all_but_pawn(int pos, const Board& board) {
	vector<int> output;
	int piece = board[pos];

	// selects a direction
	for (int direction : piece_move[piece]) {
		// starts from pos
		int new_pos = pos;

		do {
			// moves following that direction
			new_pos += direction;

			// if bedrock --> stops moving from this direction
			if (board[new_pos] == BEDROCK) {
				break;
			}
			//if (empty) --> add the new possible location
			if (board[new_pos] == EMPTY) {
				output.push_back(new_pos);
			}
			else { //if not empty
			 // if different color --> add the capture
				if (!(CHECK_SAME_COLOR(
					board[pos],
					board[new_pos]))) {
					output.push_back(new_pos);
				}
				break;
			}
		} while (repeated_piece[board[pos]]);
	}
	return output;
}

// Finds all moves for a pawn located at pos
vector<int> move_pawn(int pos, const Board& board) {
	vector<int> output;
	int new_pos = pos;
	// Selects the pawn direction
	int direction = piece_move[board[pos]][0];
	// Make the one-step move
	new_pos += direction;
	// if the new location is empty, then makes a move 
	if (board[new_pos] == EMPTY) {
		output.push_back(new_pos);
		new_pos += direction;
		int row = pos / BOARD_SIZE;
		bool double_move = (board[pos] < 0)
			? row == 3
			: row == 8;
		// checks if a double move allowed.
		if (board[new_pos] == EMPTY && double_move) {
			output.push_back(new_pos);

		}
	}

	// checks captures diagonally 
	new_pos = pos + direction;
	for (int diag : {-1, 2}) {
		new_pos += diag;
		if (board[new_pos] == EMPTY || board[new_pos] == BEDROCK) {
			continue;
		}
		if (!(CHECK_SAME_COLOR(board[new_pos], board[pos]))) {
			output.push_back(new_pos);
		}
	}
	return output;
}


// gets all possible moves for one color
vector<One_move> get_all_moves(const Board& board,
	int color,
	vector<One_move>& output) {
	output.clear();
	for (int pos : non_bedrock_cells) {
		int value = board[pos];

		if (value == EMPTY) {
			continue;
		}

		if (!(CHECK_SAME_COLOR(value, color))) {
			continue;
		}

		// find the possible moves for pos. If piece at pos is pawn, then use move_pawn. Otherwise use move_all_but_pawn.
		vector<int> moves = (abs(value) == PAWN)
			? move_pawn(pos, board)
			: move_all_but_pawn(pos, board);
		for (int new_pos : moves) {
			// if it is a pawn
			if (abs(value) == PAWN) {
				int row = new_pos / BOARD_SIZE;
				// if it gets to the other sides
				if (row == 2 || row == 9) {
					// then, promotes
					vector<int> t = (color < 0)
						? black_promote
						: white_promote;
					for (int promoting_value : t) {
						output.push_back(
							{ pos, new_pos, promoting_value });
					}
					continue;
				}
			}
			// if neither (pawn nor promotes) --> add new_move to output
			output.push_back({ pos, new_pos, board[pos] });
		}
	}

	sort(output.begin(), output.end(),
		[&board, &color](const One_move& m1, const One_move& m2) {
			int move_one_value = MOVE_SCORE(m1, board);
			int move_two_value = MOVE_SCORE(m2, board);
			return (color == WHITE)
				? (move_one_value > move_two_value)
				: (move_one_value < move_two_value);
		});
	return output;
}


// converts from a position to board notation.
string location_to_notation(int pos) {
	char row = 8 - (pos / BOARD_SIZE - 2) + '0';
	char col = pos % BOARD_SIZE - 2 + 'a';
	string output;
	output += col;
	output += row;
	return output;
}

// converts from notation to position (e.g., e8 -> );
int to_loc(char file, char rank) {
	int row = (7 - (rank - '1')) + 2;
	int col = (file - 'a') + 2;
	return row * BOARD_SIZE + col;
}

// converts from input notations to One_move, e8e7 -> {}
One_move notation_to_location(string s, const Board& board, int color) {
	return {
		to_loc(s[0], s[1]),
		to_loc(s[2], s[3]),
		(s.size() > 4)
		? color * promote[s[5]]
		: board[to_loc(s[0], s[1])]
	};
}

// converts from one PGN to One_move
One_move get_PGN(string s, const Board& board, int color, vector<One_move>& moves) {
	int new_pos = to_loc(s[1], s[2]);
	char piece_char = toupper(s[0]);
	bool found = false;
	One_move unique_move = { -1,-1,-1 };
	for (One_move move : moves) {
		if (move.end == new_pos &&
			piece_to_print[board[move.start]][1] == piece_char) {
			if (found) {
				cout << "non unique move " << endl;
				return { -1,-1,-1 };
			}
			unique_move = move;
			found = true;
		}
	}
	return unique_move;
}

// gets move from users
One_move get_users_notation(Board& board, int color) {
	vector<One_move> tmp;
	get_all_moves(board, color, tmp);
	cout << "\n";
	while (true) {
		string s;
		cout << "Please Move!\n";
		cin >> s;

		//castle
		if (s[0] == 'O') {
			One_move king_move = notation_to_location(((s.length() == 2) ? "e8g8" : "e8c8"), board, color);
			board[king_move.end] = board[king_move.start];
			board[king_move.start] = EMPTY;
			return
				notation_to_location(((s.length() == 2) ? "h8f8" : "a8d8"), board, color);
		}

		// pawn moves
		if (s.size() == 2) {
			s = "P" + s;
		}


		One_move humans_move = (s.size() == 3)
			? get_PGN(s, board, color, tmp)
			: humans_move = notation_to_location(s, board, color);

		if (find(tmp.begin(), tmp.end(), humans_move) != tmp.end()) {
			return humans_move;
		}
		cout << "INVALID MOVE \n";
	}
}

// finds the best score for white
int find_best_score_for_white(
	Board& board,
	int depth,
	int best_white_score,
	int best_black_score) {
	vector<One_move> moves;
	int best_score = std::numeric_limits<int>::min();
	//get all possible move for WHITE
	get_all_moves(board, WHITE, moves);

	for (const One_move& move : moves) {
		if (board[move.end] == -KING) {
			return std::numeric_limits<int>::max();
		}
	}

	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];

		//makes the move on the board
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		// if depth = MAX_DEPTH, --> return the direct get_score of the board. 
		int score =
			(depth == MAX_DEPTH)
			? get_score(board)
			: find_best_score_for_black(
				board,
				depth + 1,
				max(best_white_score, best_score),
				best_black_score);

		// reverses the move
		board[move.start] = start_value;
		board[move.end] = end_value;
		/*
		prunes if there is no need to scan for other branches
		as the current score of white is worse than the best
		black score. So black will never go in this direction.
		*/
		if (score >= best_black_score) {
			return std::numeric_limits<int>::max();
		}

		// updates the best score
		if (score > best_score) {
			best_score = score;
		}
	}
	return best_score;
}


// finds the best move for white
One_move find_best_move_for_white(Board& board) {
	vector<One_move> moves;
	One_move best_move = { -1,-1,1 };
	int best_score = std::numeric_limits<int>::min();

	//gets all possible moves for WHITE
	get_all_moves(board, WHITE, moves);

	// if KING can be captured --> takes that move.
	for (const One_move& move : moves) {
		if (board[move.end] == -KING) {
			return move;
		}
	}

	// scans all possible moves and find the one with the best score.
	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];

		//makes the move
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		int score = find_best_score_for_black(
			board,
			0,
			best_score,
			std::numeric_limits<int>::max());


		// reverses the move
		board[move.start] = start_value;
		board[move.end] = end_value;

		// update the best score
		if (score > best_score
			|| (score == best_score && rand() % 2 == 0)) {
			best_move = move;
			best_score = score;
			cout << location_to_notation(move.start) << location_to_notation(move.end) << ":" << score << "\n";

		}

	}
	cout << "best score = " << best_score << "\n";
	return best_move;
}

// finds the best score for black
int find_best_score_for_black(Board& board, int depth,
	int best_white_score, int best_black_score) {
	vector<One_move> moves;
	int best_score = std::numeric_limits<int>::max();
	//gets all possible move for WHITE
	get_all_moves(board, BLACK, moves);

	// if a move contain capturing black king -> return that move
	for (const One_move& move : moves) {
		if (board[move.end] == KING) {
			return std::numeric_limits<int>::min();
		}
	}


	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];

		//makes the move on the board
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		// if depth = MAX_DEPTH, 
		//      --> return the direct get_score of the board. 
		// Otherwise, we calculate the score from the white move
		int score =
			(depth == MAX_DEPTH)
			? get_score(board)
			: find_best_score_for_white(
				board,
				depth + 1,
				best_white_score,
				min(best_black_score, best_score));

		// reverses the move
		board[move.start] = start_value;
		board[move.end] = end_value;

		/*prunes if there is no need to scan for other branches
		as the score of black is worse than the best
		white score. So white will never go in this direction.
		*/
		if (score <= best_white_score) {
			return std::numeric_limits<int>::min();
		}

		// updates the best score
		if (score < best_score) {
			best_score = score;
		}
	}
	return best_score;
}


// finds the best move for white
One_move find_best_move_for_black(Board& board) {
	vector<One_move> moves;

	int best_score = std::numeric_limits<int>::max();
	//get all possible move for WHITE
	get_all_moves(board, BLACK, moves);

	for (const One_move& move : moves) {
		if (board[move.end] == KING) {
			return move;
		}
	}

	One_move best_move{ -1,-1,-1 };
	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];
		//make the move on board
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		// gets the score for the move
		int score = find_best_score_for_white(
			board,
			0,
			std::numeric_limits<int>::min(),
			best_score);

		// reverses the move
		board[move.start] = start_value;
		board[move.end] = end_value;

		// updates the best score
		if (score < best_score
			|| (score == best_score && rand() % 2 == 0)) {
			best_move = move;
			best_score = score;
		}
	}
	return best_move;
}

// Carrys the move
void carrying_move(Board& board, One_move move) {
	if (board[move.end] != EMPTY || abs(board[move.start]) == PAWN) {
		board_scores.clear();
	}
	board[move.start] = EMPTY;
	board[move.end] = move.value;
}


void play_game_simple_for_white() {
	Board board = create_initial_board();
	//Board board = load();
	int dcount = 0;
	while (true) {
		ncount = 0;
		print(board);
		One_move white_move = find_best_move_for_white(board);
		carrying_move(board, white_move);
		cout << "move: " << location_to_notation(white_move.start) << location_to_notation(white_move.end) << endl;
		print(board);
		cout << "\nNumber of possibilities = " << ncount << ". number of score board = " << board_scores.size() << "\n";
		One_move human_move = get_users_notation(board, BLACK);
		carrying_move(board, human_move);
		print(board);
		cout << "\n";
		dcount++;
		if (dcount == 15) {
			MAX_DEPTH = 6;
		}
	}
}

void play_game_simple() {
	//Board board = load();
	Board board = create_initial_board();
	One_move best_move;
	int old_score = -1;
	while (!finish(board)) {
		best_move = find_best_move_for_white(board);
		board[best_move.start] = EMPTY;
		board[best_move.end] = best_move.value;
		print(board);
		if (finish(board)) {
			break;
		}
		best_move = find_best_move_for_black(board);
		board[best_move.start] = EMPTY;
		board[best_move.end] = best_move.value;
		print(board);
	}
	print(board);
}





int main() {
	srand(time(NULL));
	non_bedrock_cells = find_non_bedrock();
	//print(load());
	play_game_simple_for_white();
}


