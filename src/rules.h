#ifndef RULES_H
#define RULES_H

#include "state.h"

int queens_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);
int rooks_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);
int bishops_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);
int check_path(game_state*, char piece, int from_row, int from_col, int to_row, int to_col, int d_row, int d_col);

int knights_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);
int pawns_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);

int kings_moves(game_state*, char piece, int from_row, int from_col, int to_row, int to_col);
int can_castle(game_state*, char piece, char side);

int is_square_valid(game_state*, char piece, int to_row, int to_col);
int is_square_empty(game_state*, int row, int col);
int is_square_foe(game_state*, char piece, int row, int col);
int is_square_friend(game_state*, char piece, int row, int col);
int is_square_threatened(game_state*, char, int, int);
int is_king_threatened(game_state*, char);

int _is_valid_move(game_state*,char,int,int,int,int);
int is_valid_move(game_state*, char,int,int,int,int);

#endif
