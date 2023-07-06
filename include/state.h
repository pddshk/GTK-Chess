#ifndef STATE_H
#define STATE_H
#include "typedecl.h"

#define STATE_OK            0
#define PAWN_ON_END_ROW     1
#define TOO_MANY_PIECES     2
#define TOO_MANY_KINGS      4
#define NO_MOVES_POSSIBLE   8
#define NO_KING             16

void init_state(game_state*);

char get_field_by_notation(game_state*, const char*);
void get_move_notation(game_state*, char*, int, int, int, int, char);
// writes two charactes into *dest *(dest+1)
void get_field_notation(int row, int col, char* dest);
char get_field(game_state*, int, int);
void set_field(game_state*, int, int, char);

int is_active(game_state*);

int is_enpassant_square(game_state*, int,int);
void set_enpassant(game_state*, int,int);
void clear_enpassant(game_state*);
void clear_castlings(game_state*);
void recalc_castlings(game_state*);
void next_move(const game_state*, char, int, int, int, int, char);

void copy_state(game_state *dest, game_state* src);

void move(game_state*, char, int, int, int, int);
void just_move(game_state*, char, int, int);
char is_castling(game_state*, char, int, int, int, int);
void castle(game_state*, char, char);
int is_pawn_promotion(char, int);
void promote_pawn(game_state*, int, int, char);
void enpassant(game_state*, char, int, int);
void cancel_drag(game_state*, char, int, int);

int is_king(char);
int is_queen(char);
int is_rook(char);
int is_bishop(char);
int is_knight(char);
int is_pawn(char);

char resolve_promotion(int);

int any_moves_possible(game_state*);
int is_mate(game_state*);
int is_stalemate(game_state*);
int fifty_moves_exceeded(game_state*);
int insufficient_material(game_state*);
//debug
void print_state(game_state*);

int validate_state(game_state*);

int castling_index(char);

#endif
