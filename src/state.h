#ifndef STATE_H
#define STATE_H

typedef struct {
    int castlings[4];
    int side_to_move;
    int fifty_moves_counter;
    int move_counter;
    char field[8][9];
    int enpassant_row, enpassant_col;
} game_state;

extern game_state state;

void init_state(game_state*);

char get_field_by_notation(game_state*, const char*);

int is_enpassant_square(game_state*, int,int);
void set_enpassant(game_state*, int,int);
void clear_enpassant(game_state*);
void recalc_castlings(game_state*);
void next_move(game_state*, char, int, int, int, int);

void copy_state(game_state*);

void move(game_state*, char, int, int, int, int);
void just_move(game_state*, char, int, int);
void castle(game_state*, char, char);
void enpassant(game_state*, char, int, int);
void cancel_drag(game_state*, char, int, int);

int is_king(char);
int is_queen(char);
int is_rook(char);
int is_bishop(char);
int is_knight(char);
int is_pawn(char);

//debug
void print_state(game_state*);

#endif
