#ifndef RULES_H
#define RULES_H

#include "state.h"

extern game_state state;

// N.B ----------------------------------------------------------------------
// make pieces not to jump over other pieces
// check threatened squares for castling
// N.B ----------------------------------------------------------------------

int queens_moves(char piece, int from_row, int from_col, int to_row, int to_col);
int rooks_moves(char piece, int from_row, int from_col, int to_row, int to_col);
int bishops_moves(char piece, int from_row, int from_col, int to_row, int to_col);
int check_path(char piece, int from_row, int from_col, int to_row, int to_col, int d_row, int d_col);

int knights_moves(char piece, int from_row, int from_col, int to_row, int to_col);
int pawns_moves(char piece, int from_row, int from_col, int to_row, int to_col);

int kings_moves(char piece, int from_row, int from_col, int to_row, int to_col);
int can_castle(char piece, char side);

int is_square_valid(char piece, int to_row, int to_col);
int is_square_empty(int row, int col);
int is_square_foe(char piece, int row, int col);
int is_square_friend(char piece, int row, int col);
//TODO
int is_square_threatened(char, int, int);
//ODOT
int is_valid_move(char,int,int,int,int);

#endif
