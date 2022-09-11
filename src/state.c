#include "state.h"
#include <string.h>
#include <stdio.h>

void init_state()
{
    strcpy(state.field[0], "rnbqkbnr");
    strcpy(state.field[1], "pppppppp");
    for (int i = 2; i < 6; strcpy(state.field[i++], "--------"));
    strcpy(state.field[6], "PPPPPPPP");
    strcpy(state.field[7], "RNBQKBNR");
    state.castlings[0] = state.castlings[1] = state.castlings[2] = state.castlings[3] = 1;
    state.side_to_move = 1;
    state.fifty_moves_counter = 0;
    state.move_counter = 0;
    state.enpassant_row = state.enpassant_col = -1;
}

char get_field_by_notation(const char* field)
{
    int i, j;
    switch (field[0]) {
        case 'a': i = 0; break;
        case 'b': i = 1; break;
        case 'c': i = 2; break;
        case 'd': i = 3; break;
        case 'e': i = 4; break;
        case 'f': i = 5; break;
        case 'g': i = 6; break;
        case 'h': i = 7; break;
    }
    j = field[1] - '0';
    return state.field[i][j];
}

void print_state(game_state* state)
{
    for (int i = 0; i < 8; i++){
        for (size_t j = 0; j < 8; j++) {
            printf("%c\t", state->field[i][j]);
        }
        printf("\n");
    }
    //printf("Enpassant is %d %d\n", state.enpassant_row, state.enpassant_col);
}

int is_enpassant_square(int row, int col)
{
    return row == state.enpassant_row && col == state.enpassant_col;
}

void set_enpassant(int row, int col)
{
    state.enpassant_col = col;
    state.enpassant_row = row;
}

void clear_enpassant()
{
    state.enpassant_row = -1;
    state.enpassant_col = -1;
}

void recalc_castlings()
{
    if (state.field[0][4] != 'k')
        state.castlings[2] = state.castlings[3] = 0;
    else {
        if (state.field[0][0] != 'r')
            state.castlings[2] = 0;
        if (state.field[0][7] != 'r')
            state.castlings[3] = 0;
    }
    if (state.field[7][4] != 'K')
        state.castlings[0] = state.castlings[1] = 0;
    else {
        if (state.field[7][0] != 'R')
            state.castlings[0] = 0;
        if (state.field[7][7] != 'R')
            state.castlings[1] = 0;
    }
}

void copy_state(game_state *other){
    memcpy((void*) other, &state, sizeof(game_state));
}
