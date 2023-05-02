#ifndef STATE_H
#define STATE_H
#include "typedecl.h"

extern state_tree tree;

extern int pawn_promotion_row, pawn_promotion_col;
extern char pawn_promotion;

void init_state(game_state*);

char get_field_by_notation(game_state*, const char*);
void get_move_notation(game_state*, char*, int, int, int, int, char);
void resolve_coord(game_state*, int*, int*);
char get_field(game_state*, int, int);
void set_field(game_state*, int, int, char);

int is_active(game_state*);

int is_enpassant_square(game_state*, int,int);
void set_enpassant(game_state*, int,int);
void clear_enpassant(game_state*);
void recalc_castlings(game_state*);
void next_move(game_state*, char, int, int, int, int, char);

void copy_state(game_state*);

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
void FEN_to_state(const char*);
void PGN_to_tree(char*);

#endif
