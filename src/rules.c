#include "rules.h"
#include <string.h>
#include <stdio.h>

int is_valid_move(char piece, int from_row, int from_col, int to_row, int to_col)
{
    if (from_row == to_row && from_col == to_col)
        return 0;
    if (!is_square_valid(piece, to_row, to_col)){
        return 0;
    }
    switch(piece){
        case 'K': case 'k':
            return kings_moves(piece, from_row, from_col, to_row, to_col);
        case 'Q': case 'q':
            return queens_moves(piece, from_row, from_col, to_row, to_col);
        case 'R': case 'r':
            return rooks_moves(piece, from_row, from_col, to_row, to_col);
        case 'B': case 'b':
            return bishops_moves(piece, from_row, from_col, to_row, to_col);
        case 'N': case 'n':
            return knights_moves(piece, from_row, from_col, to_row, to_col);
        case 'P': case 'p':
            return pawns_moves(piece, from_row, from_col, to_row, to_col);
        default:
            return 0;
    }
}

int check_path(char piece, int from_row, int from_col, int to_row, int to_col, int d_row, int d_col)
{
    printf("%c from %d %d to %d %d\n", piece, from_row, from_col, to_row, to_col);
    int dd_row = (d_row > 0) - (d_row < 0),
        dd_col = (d_col > 0) - (d_col < 0),
        blocked = 0;
    printf("%d %d\n", dd_row, dd_col);
    while(from_col != to_col || from_row != to_row) {
        if (blocked) return 0;
        from_row += dd_row;
        from_col += dd_col;
        printf("checking square %d %d\n", from_row, from_row);
        if (is_square_friend(piece, from_row, from_col))
            return 0;
        else if (is_square_foe(piece, from_row, from_col)){
            blocked = 1;
        }
    }
    return 1;
}

int queens_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == 0 || d_col == 0 || d_row == d_col || d_row == -d_col)
        return check_path(piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int rooks_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == 0 || d_col == 0)
        return check_path(piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int bishops_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (d_row == -d_col || d_row == d_col)
        return check_path(piece, from_row, from_col, to_row, to_col, d_row, d_col);
    else return 0;
}

int knights_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    return (((d_row == 2 || d_row == -2) && (d_col == 1 || d_col == -1)) ||
           ((d_col == 2 || d_col == -2) && (d_row == 1 || d_row == -1))) &&
           is_square_valid(piece, to_row, to_col);
}

int pawns_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    if (piece == 'P')
        if (from_row == 6 && d_row == -2)
            return d_col == 0 &&
                is_square_empty(from_row-1, from_col) &&
                is_square_empty(to_row, to_col);
        else
            return d_row == -1 && (
                (d_col == 0 && is_square_empty(to_row, to_col)) ||
                ((d_col == 1 || d_col == -1) &&
                (is_square_foe(piece, to_row, to_col) ||
                is_enpassant_square(to_row, to_col)))
            );
    else
        if (from_row == 1 && d_row == 2)
            return d_col == 0 &&
                is_square_empty(from_row+1, from_col) &&
                is_square_empty(to_row, to_col);
        else
            return d_row == 1 && (                          // move 1 field forward
                (d_col == 0 && is_square_empty(to_row, to_col)) || // just move
                ((d_col == 1 || d_col == -1) &&             //capture only
                (is_square_foe(piece, to_row, to_col) ||  //foe piece or
                is_enpassant_square(to_row, to_col)))       //enpassant
            );
}

int kings_moves(char piece, int from_row, int from_col, int to_row, int to_col){
    int d_row = to_row - from_row, d_col = to_col - from_col;
    return
        ((d_row == 0 && d_col == -2 && can_castle(piece, 'Q')) ||
        (d_row == 0 && d_col == 2 && can_castle(piece, 'K')) ||
        (-1 <= d_row && d_row <= 1 && -1 <= d_col && d_col <= 1)) &&
        !is_square_threatened(piece, to_row, to_col);
}

int can_castle(char piece, char side)
{
    if (piece == 'K' && side == 'Q')
        return state.castlings[0] &&
            is_square_empty(7,1) &&
            is_square_empty(7,2) &&
            is_square_empty(7,3) &&
            !is_square_threatened(piece,7,2) &&
            !is_square_threatened(piece,7,3);
    if (piece == 'K' && side == 'K')
        return
            state.castlings[1] &&
            is_square_empty(7,5) &&
            is_square_empty(7,6) &&
            !is_square_threatened(piece,7,5) &&
            !is_square_threatened(piece,7,6);
    if (piece == 'k' && side == 'Q')
        return
            state.castlings[2] &&
            is_square_empty(0,1) &&
            is_square_empty(0,2) &&
            is_square_empty(0,3) &&
            !is_square_threatened(piece,0,2) &&
            !is_square_threatened(piece,0,3);
    if (piece == 'k' && side == 'K')
        return
            state.castlings[3] &&
            is_square_empty(0,5) &&
            is_square_empty(0,6) &&
            !is_square_threatened(piece,0,5) &&
            !is_square_threatened(piece,0,6);
    return 0;
}

int is_castling(char piece, int from_row, int from_col, int to_row, int to_col)
{
    return (piece == 'K' || piece == 'k') &&
        (from_col - to_col == 2 || from_col - to_col == -2);
}

int is_square_valid(char piece, int row, int col){
    return is_square_empty(row, col) || is_square_foe(piece, row, col);
}

int is_square_empty(int row, int col)
{
    return state.field[row][col] == '-';
}

int is_square_foe(char piece, int row, int col){
    if (strchr("KQNBRP", piece))
        return !!strchr("kqnbrp", state.field[row][col]);
    else
        return !!strchr("KQNBRP", state.field[row][col]);
}

int is_square_friend(char piece, int row, int col){
    if (strchr("KQNBRP", piece))
        return !!strchr("KQNBRP", state.field[row][col]);
    else
        return !!strchr("kqnbrp", state.field[row][col]);
}

//TODO
int is_square_threatened(char piece, int row, int col)
{
    return 0;
}
