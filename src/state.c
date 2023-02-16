#include "state.h"
#include "state_tree.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rules.h"
#include "gui.h"

extern state_tree* tree;

void init_state(game_state* state)
{
    memcpy(
        state->field[0],
        "rnbqkbnr\0pppppppp\0--------\0--------\0--------\0--------\0PPPPPPPP\0RNBQKBNR\0",
        8*9
    );
    state->castlings[0] = state->castlings[1] = state->castlings[2] = state->castlings[3] = 1;
    state->side_to_move = 1;
    state->fifty_moves_counter = 0;
    state->move_counter = 1;
    state->enpassant_row = state->enpassant_col = -1;
    state->is_active = 1;
    state->flipped = 0;
}

int is_active(game_state* state){
    return state->is_active;
}

char get_field_by_notation(game_state* state, const char* field)
{
    int i = field[0] - 'a', j = field[1] - '0';
    return get_field(state, i, j);
}

void get_move_notation(game_state* state, char* res, int from_row, int from_col, int to_row, int to_col, char promoted)
{
    res[0] = from_col + 'a';
    res[1] = 8 - from_row + '0';
    res[2] = to_col + 'a';
    res[3] = 8 - to_row + '0';
    res[4] = promoted;
}

void resolve_coord(game_state* state, int*row, int*col)
{
    if (state->flipped){
        *row = 7 - *row;
        *col = 7 - *col;
    }
}

int get_field(game_state* state, int row, int col)
{
    if (state->flipped)
        return state->field[7-row][7-col];
    else
        return state->field[row][col];
}

void set_field(game_state* state, int row, int col, char piece)
{
    if (state->flipped)
        state->field[7-row][7-col] = piece;
    else
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

void next_move(game_state* state, char piece, int from_row, int from_col, int to_row, int to_col, char promotion)
{
    state->side_to_move = !state->side_to_move;
    if (state->side_to_move) {
        state->move_counter++;
    }
    if (is_pawn(piece) || is_square_foe(state, piece, to_row, to_col))
        state->fifty_moves_counter = 0;
    else
        state->fifty_moves_counter++;
    move(state, promotion ? promotion : piece, from_row, from_col, to_row, to_col);
    recalc_castlings(state);
    if (piece == 'P' && to_row - from_row == -2)
        set_enpassant(state, to_row+1, to_col);
    else if (piece == 'p' && to_row - from_row == 2)
        set_enpassant(state, to_row-1, to_col);
    else
        clear_enpassant(state);
    char *move = malloc(sizeof(char) * 6);
    get_move_notation(state, move, from_row, from_col, to_row, to_col, promotion);
    
    //
    game_state* state_storage = (game_state*) malloc(sizeof(game_state));
    *state_storage = *state;
    (*tree).current =  addnode(state_storage, tree->current, move); 
    show_state(tree->root,0);
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
    set_field(state, from_row, from_col, piece);
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
                case 'R': case 'r':
                    for (int k = 0; k < 8; k++){
                        if (is_valid_move(state, piece, i, j, i+k, j) ||
                            is_valid_move(state, piece, i, j, i, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j) ||
                            is_valid_move(state, piece, i, j, i, j-k)
                        )
                            return 1;
                    }
                case 'B': case 'b':
                    for (int k = 0; k < 8; k++){
                        if (is_valid_move(state, piece, i, j, i+k, j+k) ||
                            is_valid_move(state, piece, i, j, i+k, j-k) ||
                            is_valid_move(state, piece, i, j, i-k, j+k) ||
                            is_valid_move(state, piece, i, j, i-k, j-k)
                        )
                            return 1;
                    }
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
                case 'P':
                    if (is_valid_move(state, piece, i, j, i-1, j-1) ||
                        is_valid_move(state, piece, i, j, i-1, j) ||
                        is_valid_move(state, piece, i, j, i-1, j+1) ||
                        is_valid_move(state, piece, i, j, i-2, j)
                    )
                        return 1;
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

void FEN_to_state(char* fen) {
    char delim[] = " ";
    game_state newstate;
    char* placement = strtok(fen, delim);
    char field_ptr = 0;
    for(int i = 0; i < strlen(placement); field_ptr++) {
        if (placement[i] < '0' || placement[i] > '9') {
            if (placement[i] == '/') {
                newstate.field[field_ptr / 9][field_ptr % 9] = '\0';
            }
            else {
                newstate.field[field_ptr / 9][field_ptr % 9] = placement[i];
            }
            i++;
        }
        else {
            placement[i]--;
            newstate.field[field_ptr / 9][field_ptr % 9] = '-';
            if (placement[i] == '0') i++;
        }
    }
    char* color = strtok(NULL, delim);
    if (color[0] == 'w') {
        newstate.side_to_move = 1;
    }
    else {
        newstate.side_to_move = 0;
    }
    char* castling = strtok(NULL, delim);
    for(int i = 0; i < strlen(castling); i++) {
        if (castling[i] == 'K') {
            newstate.castlings[1] = 1;
        }
        if (castling[i] == 'Q') {
            newstate.castlings[0] = 1;
        }
        if (castling[i] == 'k') {
            newstate.castlings[3] = 1;
        }
        if (castling[i] == 'Q') {
            newstate.castlings[2] = 1;
        }
    }
    char* enpassant = strtok(NULL, delim);
    if (enpassant[0] != '-') {
        newstate.enpassant_col = enpassant[0] - 'a';
        newstate.enpassant_row = enpassant[1] - '0';
    }
    char* fiftymoves = strtok(NULL, delim);
    sscanf(fiftymoves, "%d", &newstate.fifty_moves_counter);
    char* fullmove = strtok(NULL, delim);
    sscanf(fullmove, "%d", &newstate.move_counter);
    state = newstate;
}

void PGN_to_tree(char* fen) 
{
    char* move_strings= strtok(fen,"\n");
    for(int i=0; i<9;i++)
    {
       move_strings = strtok(NULL,"\n");
    }
    move_strings = strtok(NULL,"\0");//string with moves
    game_state newstate;

    printf("%s\n",move_strings);
    /*while(move_strings != NULL)
    {
        printf("%s\n",move_strings);
        move_strings= strtok(NULL,"\n");
    }*/
    //move_strings = strtok(NULL," \n");
    //printf("324%s\n",move_strings);
    
    
    
    /*char delim[] = " ";
    game_state newstate;
    char* placement = strtok(fen, delim);
    char field_ptr = 0;
    for(int i = 0; i < strlen(placement); field_ptr++) {
        if (placement[i] < '0' || placement[i] > '9') {
            if (placement[i] == '/') {
                newstate.field[field_ptr / 9][field_ptr % 9] = '\0';
            }
            else {
                newstate.field[field_ptr / 9][field_ptr % 9] = placement[i];
            }
            i++;
        }
        else {
            placement[i]--;
            newstate.field[field_ptr / 9][field_ptr % 9] = '-';
            if (placement[i] == '0') i++;
        }
    }
    char* color = strtok(NULL, delim);
    if (color[0] == 'w') {
        newstate.side_to_move = 1;
    }
    else {
        newstate.side_to_move = 0;
    }
    char* castling = strtok(NULL, delim);
    for(int i = 0; i < strlen(castling); i++) {
        if (castling[i] == 'K') {
            newstate.castlings[1] = 1;
        }
        if (castling[i] == 'Q') {
            newstate.castlings[0] = 1;
        }
        if (castling[i] == 'k') {
            newstate.castlings[3] = 1;
        }
        if (castling[i] == 'Q') {
            newstate.castlings[2] = 1;
        }
    }
    char* enpassant = strtok(NULL, delim);
    if (enpassant[0] != '-') {
        newstate.enpassant_col = enpassant[0] - 'a';
        newstate.enpassant_row = enpassant[1] - '0';
    }
    char* fiftymoves = strtok(NULL, delim);
    sscanf(fiftymoves, "%d", &newstate.fifty_moves_counter);
    char* fullmove = strtok(NULL, delim);
    sscanf(fullmove, "%d", &newstate.move_counter);
    state = newstate;*/
}
