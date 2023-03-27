#ifndef TYPEDECL_H
#define TYPEDECL_H

typedef struct {
    int castlings[4]; // Queenside | Kingside | queenside | kingside
    int side_to_move;
    int fifty_moves_counter;
    int move_counter;
    char field[8][9];
    // set when pawn is moved 2 steps forward
    // unset when not
    int enpassant_row, enpassant_col;
    int is_active;
    int flipped;
} game_state;

#endif
