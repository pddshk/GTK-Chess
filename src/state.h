#ifndef STATE_H
#define STATE_H

typedef struct {
    int castlings[4];
    int side_to_move;
    int fifty_moves_counter;
    int move_counter;
    char field[8][9];
    char enpassant[3];
} board_state;

extern board_state game_state;

void init_state();

char get_field(const char*);

#endif
