#include "state.h"
#include <string.h>
#include <stdio.h>
#include "rules.h"

void init_state(game_state* state)
{
    strcpy(state->field[0], "rnbqkbnr");
    strcpy(state->field[1], "pppppppp");
    for (int i = 2; i < 6; strcpy(state->field[i++], "--------"));
    strcpy(state->field[6], "PPPPPPPP");
    strcpy(state->field[7], "RNBQKBNR");
    state->castlings[0] = state->castlings[1] = state->castlings[2] = state->castlings[3] = 1;
    state->side_to_move = 1;
    state->fifty_moves_counter = 0;
    state->move_counter = 0;
    state->enpassant_row = state->enpassant_col = -1;
}

char get_field_by_notation(game_state* state, const char* field)
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
    return state->field[i][j];
}

void print_state(game_state* state)
{
    for (int i = 0; i < 8; i++){
        for (size_t j = 0; j < 8; j++) {
            printf("%c\t", state->field[i][j]);
        }
        printf("\n");
    }
}

int is_enpassant_square(game_state *state, int row, int col)
{
    return row == state->enpassant_row && col == state->enpassant_col;
}

void set_enpassant(game_state* state, int row, int col)
{
    state->enpassant_col = col;
    state->enpassant_row = row;
}

void clear_enpassant(game_state *state)
{
    state->enpassant_row = state->enpassant_col = -1;
}

void recalc_castlings(game_state* state)
{
    if (state->field[0][4] != 'k')
        state->castlings[2] = state->castlings[3] = 0;
    else {
        if (state->field[0][0] != 'r')
            state->castlings[2] = 0;
        if (state->field[0][7] != 'r')
            state->castlings[3] = 0;
    }
    if (state->field[7][4] != 'K')
        state->castlings[0] = state->castlings[1] = 0;
    else {
        if (state->field[7][0] != 'R')
            state->castlings[0] = 0;
        if (state->field[7][7] != 'R')
            state->castlings[1] = 0;
    }
}

void next_move(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    state->side_to_move = !state->side_to_move;
    state->move_counter++;
    if (is_pawn(piece) || is_square_foe(state, piece, to_row, to_col))
        state->fifty_moves_counter = 0;
    else
        state->fifty_moves_counter++;
    move(state, piece, from_row, from_col, to_row, to_col);
    recalc_castlings(state);
    if (piece == 'P' && to_row - from_row == -2)
        set_enpassant(state, to_row+1, to_col);
    else if (piece == 'p' && to_row - from_row == 2)
        set_enpassant(state, to_row-1, to_col);
    else
        clear_enpassant(state);

}

void move(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    if (is_enpassant_square(state, to_row, to_col)){
        enpassant(state, piece, to_row, to_col);
    }
    char castling_side = is_castling(state, piece, from_row, from_col, to_row, to_col);
    if (castling_side != '-'){
        castle(state, piece, castling_side);
    } else if (is_pawn_promotion(piece, to_row)) {
        pawn_promotion = piece;
        pawn_promotion_row = to_row;
        pawn_promotion_col = to_col;
    } else {
        just_move(state, piece, to_row, to_col);
    }
}

void just_move(game_state* state, char piece, int to_row, int to_col)
{
    state->field[to_row][to_col] = piece;
}

char is_castling(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    int d_col = to_col - from_col;
    if (piece == 'K' || piece == 'k'){
        if (d_col == -2)
            return 'Q';
        else if (d_col == 2)
            return 'K';
    }
    return '-';
}

void castle(game_state* state, char king, char side)
{
    switch (king) {
        case 'K':
            switch (side) {
                case 'Q':
                    state->field[7][4] = '-';
                    state->field[7][2] = 'K';
                    state->field[7][0] = '-';
                    state->field[7][3] = 'R';
                    return;
                case 'K':
                    state->field[7][4] = '-';
                    state->field[7][6] = 'K';
                    state->field[7][7] = '-';
                    state->field[7][5] = 'R';
                    return;
            }
        case 'k':
            switch (side) {
                case 'Q':
                    state->field[0][4] = '-';
                    state->field[0][2] = 'k';
                    state->field[0][0] = '-';
                    state->field[0][3] = 'r';
                    return;
                case 'K':
                    state->field[0][4] = '-';
                    state->field[0][6] = 'k';
                    state->field[0][7] = '-';
                    state->field[0][5] = 'r';
                    return;

            }
    }
}

int is_pawn_promotion(char piece, int to_row)
{
    return (piece == 'P' && to_row == 0) ||
            (piece == 'p' && to_row == 7);
}

void promote_pawn(game_state* state, int to_row, int to_col, char newpiece)
{
    state->field[to_row][to_col] = newpiece;
}

void enpassant(game_state* state, char piece, int to_row, int to_col){
    just_move(state, piece, to_row, to_col);
    switch (piece) {
        case 'P':
            state->field[to_row+1][to_col] = '-';
            return;
        case 'p':
            state->field[to_row-1][to_col] = '-';
            return;
        default: // triggers if other piece move to passed square
            return;
    }
}

void cancel_drag(game_state* state, char piece, int from_row, int from_col)
{
    state->field[from_row][from_col] = piece;
}

int is_king(char piece) { return piece == 'K' || piece == 'k'; }
int is_queen(char piece) { return piece == 'Q' || piece == 'q'; }
int is_rook(char piece) { return piece == 'R' || piece == 'r'; }
int is_bishop(char piece) { return piece == 'B' || piece == 'b'; }
int is_knight(char piece) { return piece == 'N' || piece == 'n'; }
int is_pawn(char piece) { return piece == 'P' || piece == 'p'; }

void copy_state(game_state *other){
    memcpy((void*) other, &state, sizeof(game_state));
}

char resolve_promotion(int row)
{
    return "QRBNnbrq"[row];
}
