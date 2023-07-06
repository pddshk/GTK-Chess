#include "state.h"
#include "state_tree.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "rules.h"
#include "gtkchessapp.h"
#include "notation.h"

// TODO: get rid of it!!
extern state_tree tree;

void init_state(game_state* state)
{
    memcpy(
        state->field[0],
        "rnbqkbnr\0pppppppp\0--------\0--------\0--------\0--------\0PPPPPPPP\0RNBQKBNR\0",
        8*9
    );
    state->castlings[0] = state->castlings[1] = state->castlings[2] = state->castlings[3] = 1;
    state->side_to_move = WHITE;
    state->fifty_moves_counter = 0;
    state->move_counter = 1;
    state->enpassant_row = state->enpassant_col = -1;
    state->is_active = 1;   
}

// game_state init_state_non_pointer()
// {
//     game_state state;
//     strcpy((char*)state.field, "rnbqkbnr\0pppppppp\0--------\0--------\0--------\0--------\0PPPPPPPP\0RNBQKBNR\0");
//     state.castlings[0] = state.castlings[1] = state.castlings[2] = state.castlings[3] = 1;
//     state.side_to_move = 1;
//     state.fifty_moves_counter = 0;
//     state.move_counter = 1;
//     state.enpassant_row = state.enpassant_col = -1;
//     state.is_active = 1;
//     state.flipped = 0;
//     return state;
// }

int is_active(game_state* state){
    return state->is_active;
}

char get_field_by_notation(game_state* state, const char* field)
{
    int i = field[0] - 'a', j = field[1] - '0';
    return get_field(state, i, j);
}

void get_move_notation(__attribute_maybe_unused__ game_state* state, char* res, int from_row, int from_col, int to_row, int to_col, char promoted)
{
    res[0] = from_col + 'a';
    res[1] = 8 - from_row + '0';
    res[2] = to_col + 'a';
    res[3] = 8 - to_row + '0';
    res[4] = promoted;
}

void get_field_notation(int row, int col, char* dest)
{
    *dest = col + 'a';
    dest++;
    *dest = 8 - row + '0';
}

char get_field(game_state* state, int row, int col) {
    return state->field[row][col];
}

void set_field(game_state* state, int row, int col, char piece) {
    state->field[row][col] = piece;
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

void clear_castlings(game_state *state){
    state->castlings[0] = state->castlings[1] = state->castlings[2] = state->castlings[3] = FALSE;
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

void next_move(__attribute_maybe_unused__ const game_state* state, char piece, int from_row, int from_col, int to_row, int to_col, char promotion)
{
    game_state new_state = *state;
    if (!promotion)
        set_field(&new_state, from_row, from_col, '-');
    
    new_state.side_to_move = !new_state.side_to_move;
    if (new_state.side_to_move) {
        new_state.move_counter++;
    }
    if (is_pawn(piece) || is_square_foe(&new_state, piece, to_row, to_col))
        new_state.fifty_moves_counter = 0;
    else
        new_state.fifty_moves_counter++;
    move(&new_state, promotion ? promotion : piece, from_row, from_col, to_row, to_col);
    recalc_castlings(&new_state);
    if (piece == 'P' && to_row - from_row == -2)
        set_enpassant(&new_state, to_row+1, to_col);
    else if (piece == 'p' && to_row - from_row == 2)
        set_enpassant(&new_state, to_row-1, to_col);
    else
        clear_enpassant(&new_state);
    char move_buffer[6] = "";
    get_move_notation(&new_state, move_buffer, from_row, from_col, to_row, to_col, promotion);
    
    //
    tree.current =  addnode(new_state, tree.current,  move_buffer); 
    show_state(tree.root,0);
    //
}

void move(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col)
{
    if (is_enpassant_square(state, to_row, to_col))
        enpassant(state, piece, to_row, to_col);
    char castling_side = is_castling(state, piece, from_row, from_col, to_row, to_col);
    if (castling_side != '-')
        castle(state, piece, castling_side);
    else
        just_move(state, piece, to_row, to_col);
}

void just_move(game_state* state, char piece, int to_row, int to_col)
{
    state->field[to_row][to_col] = piece;
}

char is_castling(
    __attribute_maybe_unused__ game_state* state,
    char piece,
    __attribute_maybe_unused__ int from_row, int from_col,
    __attribute_maybe_unused__ int to_row, int to_col)
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
            break;
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
            break;
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
    set_field(state, from_row, from_col, piece);
}

int is_king(char piece) { return piece == 'K' || piece == 'k'; }
int is_queen(char piece) { return piece == 'Q' || piece == 'q'; }
int is_rook(char piece) { return piece == 'R' || piece == 'r'; }
int is_bishop(char piece) { return piece == 'B' || piece == 'b'; }
int is_knight(char piece) { return piece == 'N' || piece == 'n'; }
int is_pawn(char piece) { return piece == 'P' || piece == 'p'; }

void copy_state(game_state *dest, game_state* src){
    memcpy((void*) dest, src, sizeof(game_state));
}

char resolve_promotion(int row)
{
    return "QRBNnbrq"[row];
}

int any_moves_possible(game_state* state)
{
    const char* piece_set = state->side_to_move ? "KQRBNP" : "kqrbnp";
    for (int i=0; i<8; i++) for (int j=0; j<8; j++) {
        char piece = state->field[i][j];
        if (strchr(piece_set, piece))
            switch (piece) {
                case 'K': case 'k':
                    return is_valid_move(state, piece, i, j, i-1, j-1) ||
                            is_valid_move(state, piece, i, j, i-1, j) ||
                            is_valid_move(state, piece, i, j, i-1, j+1) ||
                            is_valid_move(state, piece, i, j, i, j-1) ||
                            is_valid_move(state, piece, i, j, i, j+1) ||
                            is_valid_move(state, piece, i, j, i+1, j-1) ||
                            is_valid_move(state, piece, i, j, i+1, j) ||
                            is_valid_move(state, piece, i, j, i+1, j+1);
                case 'Q': case 'q':
                    for (int k = 0; k < 8; k++){
                        if (is_valid_move(state, piece, i, j, i+k, j) ||
                            is_valid_move(state, piece, i, j, i, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j) ||
                            is_valid_move(state, piece, i, j, i, j-k) ||
                            is_valid_move(state, piece, i, j, i+k, j+k) ||
                            is_valid_move(state, piece, i, j, i+k, j-k) ||
                            is_valid_move(state, piece, i, j, i-k, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j-k)
                        )
                            return 1;
                    }
                    break;
                case 'R': case 'r':
                    for (int k = 0; k < 8; k++){
                        if (is_valid_move(state, piece, i, j, i+k, j) ||
                            is_valid_move(state, piece, i, j, i, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j) ||
                            is_valid_move(state, piece, i, j, i, j-k)
                        )
                            return 1;
                    }
                    break;
                case 'B': case 'b':
                    for (int k = 0; k < 8; k++){
                        if (is_valid_move(state, piece, i, j, i+k, j+k) ||
                            is_valid_move(state, piece, i, j, i+k, j-k) ||
                            is_valid_move(state, piece, i, j, i-k, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j-k)
                        )
                            return 1;
                    }
                    break;
                case 'N': case 'n':
                    if (is_valid_move(state, piece, i, j, i-2, j-1) ||
                        is_valid_move(state, piece, i, j, i-2, j+1) ||
                        is_valid_move(state, piece, i, j, i-1, j-2) ||
                        is_valid_move(state, piece, i, j, i-1, j+2) ||
                        is_valid_move(state, piece, i, j, i+1, j-2) ||
                        is_valid_move(state, piece, i, j, i+1, j+2) ||
                        is_valid_move(state, piece, i, j, i+2, j-1) ||
                        is_valid_move(state, piece, i, j, i+2, j+1)
                    )
                        return 1;
                    break;
                case 'P':
                    if (is_valid_move(state, piece, i, j, i-1, j-1) ||
                        is_valid_move(state, piece, i, j, i-1, j) ||
                        is_valid_move(state, piece, i, j, i-1, j+1) ||
                        is_valid_move(state, piece, i, j, i-2, j)
                    )
                        return 1;
                    break;
                case 'p':
                    if (is_valid_move(state, piece, i, j, i+1, j-1) ||
                        is_valid_move(state, piece, i, j, i+1, j) ||
                        is_valid_move(state, piece, i, j, i+1, j+1) ||
                        is_valid_move(state, piece, i, j, i+2, j)
                    )
                        return 1;
            }
    }
    return 0;
}

int is_mate(game_state *state)
{
    return !any_moves_possible(state) &&
        is_king_threatened(state, state->side_to_move ? 'K' : 'k');
}

int is_stalemate(game_state *state)
{
    return !any_moves_possible(state) &&
        !is_king_threatened(state, state->side_to_move ? 'K' : 'k');
}

int fifty_moves_exceeded(game_state* state)
{
    return state->fifty_moves_counter > 49;
}

int insufficient_material(game_state* state)
{
    int bn = 0, wn = 0, bbb = 0, bwb = 0, wbb = 0, wwb = 0;
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++){
        switch (state->field[i][j]) {
            case 'Q': case 'q': case 'R': case 'r': case 'P': case 'p':
                return 0;
            case 'N':
                wn++;
                break;
            case 'n':
                bn++;
                break;
            case 'B':
                if ((i+j)%2)
                    wwb++;
                else
                    wbb++;
                break;
            case 'b':
                if ((i+j)%2)
                    bwb++;
                else
                    bbb++;
                break;
        }
    }
    int whites = wn + wbb + wwb, blacks = bn + bwb + bbb;
    return whites * blacks == 0 &&
        ((wn == 0 && wbb*wwb == 0) || (bn == 0 && bbb*bwb == 0));
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

int validate_state(game_state* state)
{
    int res = STATE_OK;
    int p=0, n=0, b=0, r=0, q=0, k=0,
        P=0, N=0, B=0, R=0, Q=0, K=0;
    for (int i=0; i<8; ++i) for(int j=0; j<8; ++j) {
        switch (state->field[i][j])
        {
        case '-': continue;
        case 'p':
            if (i == 0 || i == 8)
                res |= PAWN_ON_END_ROW;
            p++; break;
        case 'P':
            if (i == 0 || i == 8)
                res |= PAWN_ON_END_ROW;
            P++; break;
        case 'n': n++; break;
        case 'N': N++; break;
        case 'b': b++; break;
        case 'B': B++; break;
        case 'r': r++; break;
        case 'R': R++; break;
        case 'q': q++; break;
        case 'Q': Q++; break;
        case 'k': k++; break;
        case 'K': K++; break;
        default: return FALSE;
        }
    }
    if (p+n+b+r+q>15 || p+n>10 || p+b>10 || p+r>10 || p+q>10 ||
        P+N+B+R+Q>15 || P+N>10 || P+B>10 || P+R>10 || P+Q>10)
        res |= TOO_MANY_PIECES;
    if (k>1 ||K>1)
        res |= TOO_MANY_KINGS;
    else if (k==0 || K==0)
        res |= NO_KING;
    if (res == STATE_OK && !any_moves_possible(state))
        res |= NO_MOVES_POSSIBLE;
    return res;
}

int castling_index(char c)
{
    switch (c)
    {
    case 'K': return 1;
    case 'Q': return 0;
    case 'k': return 3;
    case 'q': return 2;
    default : return -1;
    }
}
