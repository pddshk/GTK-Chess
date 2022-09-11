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

void init_state();

char get_field_by_notation(const char*);

int is_enpassant_square(int,int);

void set_enpassant(int,int);

void clear_enpassant();

void recalc_castlings();

void copy_state(game_state*);

//debug
void print_state(game_state*);

#endif
