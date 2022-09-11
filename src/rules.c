#include "rules.h"
#include <string.h>
#include <stdio.h>

int _is_valid_move(game_state *state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    if (from_row == to_row && from_col == to_col)
        return 0;
    if (!is_square_valid(state, piece, to_row, to_col))
        return 0;
    switch(piece){
        case 'K': case 'k':
            return kings_moves(state, piece, from_row, from_col, to_row, to_col);
        case 'Q': case 'q':
            return queens_moves(state, piece, from_row, from_col, to_row, to_col);
        case 'R': case 'r':
            return rooks_moves(state, piece, from_row, from_col, to_row, to_col);
        case 'B': case 'b':
            return bishops_moves(state, piece, from_row, from_col, to_row, to_col);
        case 'N': case 'n':
            return knights_moves(state, piece, from_row, from_col, to_row, to_col);
        case 'P': case 'p':
            return pawns_moves(state, piece, from_row, from_col, to_row, to_col);
        default:
            return 0;
    }
}

int is_valid_move(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    if (_is_valid_move(state, piece, from_row, from_col, to_row, to_col)){
        game_state possible_state;
        copy_state(&possible_state);
        possible_state.field[to_row][to_col] = piece;
        possible_state.field[from_row][from_col] = '-';
        if (is_king_threatened(&possible_state, state->side_to_move ? 'K' : 'k')){
            return 0;
        }
        return 1;
    }
    return 0;
}

int check_path(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col, int d_row, int d_col)
{
    int dd_row = (d_row > 0) - (d_row < 0),
        dd_col = (d_col > 0) - (d_col < 0),
        blocked = 0;
    while(from_col != to_col || from_row != to_row) {
        if (blocked) return 0;
        from_row += dd_row;
        from_col += dd_col;
        if (is_square_friend(state, piece, from_row, from_col))
            return 0;
        else if (is_square_foe(state, piece, from_row, from_col)){
            blocked = 1;
        }
    }
    return 1;
}

int queens_moves(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == 0 || d_col == 0 || d_row == d_col || d_row == -d_col)
        return check_path(state, piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int rooks_moves(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == 0 || d_col == 0)
        return check_path(state, piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int bishops_moves(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == -d_col || d_row == d_col)
        return check_path(state, piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int knights_moves(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    return (((d_row == 2 || d_row == -2) && (d_col == 1 || d_col == -1)) ||
           ((d_col == 2 || d_col == -2) && (d_row == 1 || d_row == -1)));
}

int pawns_moves(game_state *state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (piece == 'P')
        if (from_row == 6 && d_row == -2)
            return d_col == 0 &&
                is_square_empty(state, from_row-1, from_col) &&
                is_square_empty(state, to_row, to_col);
        else
            return d_row == -1 && (
                (d_col == 0 && is_square_empty(state, to_row, to_col)) ||
                ((d_col == 1 || d_col == -1) &&
                (is_square_foe(state, piece, to_row, to_col) ||
                is_enpassant_square(state, to_row, to_col)))
            );
    else
        if (from_row == 1 && d_row == 2)
            return d_col == 0 &&
                is_square_empty(state, from_row+1, from_col) &&
                is_square_empty(state, to_row, to_col);
        else
            return d_row == 1 && (                          // move 1 field forward
                (d_col == 0 && is_square_empty(state, to_row, to_col)) || // just move
                ((d_col == 1 || d_col == -1) &&             //capture only
                (is_square_foe(state, piece, to_row, to_col) ||  //foe piece or
                is_enpassant_square(state, to_row, to_col)))       //enpassant
            );
}

int kings_moves(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    return
        ((d_row == 0 && d_col == -2 && can_castle(state, piece, 'Q')) ||
        (d_row == 0 && d_col == 2 && can_castle(state, piece, 'K')) ||
        (-1 <= d_row && d_row <= 1 && -1 <= d_col && d_col <= 1)) &&
        !is_square_threatened(state, piece, to_row, to_col);
}

int can_castle(game_state* state, char piece, char side)
{
    if (piece == 'K' && side == 'Q')
        return state->castlings[0] &&
            is_square_empty(state, 7,1) &&
            is_square_empty(state, 7,2) &&
            is_square_empty(state, 7,3) &&
            !is_square_threatened(state, piece,7,2) &&
            !is_square_threatened(state, piece,7,3) &&
            !is_square_threatened(state, piece,7,4);
    if (piece == 'K' && side == 'K')
        return
            state->castlings[1] &&
            is_square_empty(state, 7,5) &&
            is_square_empty(state, 7,6) &&
            !is_square_threatened(state, piece,7,4) &&
            !is_square_threatened(state, piece,7,5) &&
            !is_square_threatened(state, piece,7,6);
    if (piece == 'k' && side == 'Q')
        return
            state->castlings[2] &&
            is_square_empty(state, 0,1) &&
            is_square_empty(state, 0,2) &&
            is_square_empty(state, 0,3) &&
            !is_square_threatened(state, piece,0,2) &&
            !is_square_threatened(state, piece,0,3) &&
            !is_square_threatened(state, piece,0,4);
    if (piece == 'k' && side == 'K')
        return
            state->castlings[3] &&
            is_square_empty(state, 0,5) &&
            is_square_empty(state, 0,6) &&
            !is_square_threatened(state, piece,0,4) &&
            !is_square_threatened(state, piece,0,5) &&
            !is_square_threatened(state, piece,0,6);
    return 0;
}

int is_square_valid(game_state *state, char piece, int row, int col){
    return is_square_empty(state, row, col) || is_square_foe(state, piece, row, col);
}

int is_square_empty(game_state *state, int row, int col)
{
    return state->field[row][col] == '-';
}

int is_square_foe(game_state *state, char piece, int row, int col){
    if (strchr("KQNBRP", piece))
        return !!strchr("kqnbrp", state->field[row][col]);
    else
        return !!strchr("KQNBRP", state->field[row][col]);
}

int is_square_friend(game_state *state, char piece, int row, int col){
    if (strchr("KQNBRP", piece))
        return !!strchr("KQNBRP", state->field[row][col]);
    else
        return !!strchr("kqnbrp", state->field[row][col]);
}

int is_square_threatened(game_state *state, char piece, int row, int col)
{
    int side = !!strchr("KQNBRP", piece);
    const char* foes = side ? "kqnbrp" : "KQNBRP";
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            char current = state->field[i][j];
            if (strchr(foes, current) && _is_valid_move(state, current, i, j, row, col))
                return 1;
        }
    }
    //check pawn attacks
    if (side) { //white
        if (row > 1){
            if (col == 0){
                if (state->field[row-1][col+1] == 'p')
                    return 1;
                else
                    return 0;
            }
            if (col == 7){
                if (state->field[row-1][col-1] == 'p')
                    return 1;
                else
                    return 0;
            }
            if (state->field[row-1][col+1] == 'p' || state->field[row-1][col-1] == 'p')
                return 1;
        }
    } else { //black
        if (row < 6){
            if (col == 0){
                if (state->field[row+1][col+1] == 'P')
                    return 1;
                else
                    return 0;
            }
            if (col == 7){
                if (state->field[row+1][col-1] == 'P')
                    return 1;
                else
                    return 0;
            }
            if (state->field[row+1][col+1] == 'P' || state->field[row+1][col-1] == 'P')
                return 1;
        }
    }
    return 0;
}

int is_king_threatened(game_state* state, char piece)
{
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
        if (state->field[i][j] == piece)
            return is_square_threatened(state, piece, i, j);
    //unreachable; suppress warning
    return -1;
}
