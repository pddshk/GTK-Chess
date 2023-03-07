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

void swap(int* mas, int a, int b)
{
    int t = mas[a];
    mas[b]=mas[a];
    mas[a]=t;
}
void min_el(int* mas, int mas_size)
{
    for(int i=0;i<mas_size;i++)
    {
        mas[i]*=-1;
    }
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
    printf("%c\n",piece);
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
    (*tree).current =  addnode(state_storage, tree->current,  move); 
    printf("%s\n",move);
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
    for(int i = 0; i < strlen(placement); field_ptr++) 
    {
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


void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}


void PGN_to_tree(char* fen) 
{

char* t_com = strchr(fen,';');
while(t_com!=NULL)
{
    int index = (int)(t_com - fen);
    while(t_com!='\n'||t_com!='\0')
    {
        t_com++;
        if(t_com==' ')
        t_com='_';
    }
    t_com=strchr(fen,';');
}
char* temp_fen = (char*)malloc((strlen(fen)+4));
sprintf(temp_fen, "%s%s"," \n",fen);


char* move_strings= strtok(temp_fen,"\n");

GList* list = NULL;
int b=0;
char* com;



while (1)
{
    char word[8];
    move_strings = strtok(NULL," \n");
    if(move_strings==NULL)
    break;

    /*else if((com =strchr(move_strings, ';')) != NULL)
    {
        
        
        //printf("com %d\n",com);
        int index = (int)(com - move_strings);
        char* temp_memory = (char*)malloc(sizeof(char)*(index));
        for(int j=0; j<index;j++)
        {
            temp_memory[j]=move_strings[j];
        }
        temp_memory[index]='\0';
        strcpy(word, temp_memory);
        
        //free(temp_memory);
        printf("%s generic string after comment\n",word);
        list=g_list_append(list, word);
        //if(move_strings[strlen(move_strings)-1]!="\n")
        //move_strings = strtok(NULL,"\n");

        
        
        //printf("com1 %d\n",com1);
        
        /*for(int r=0;r<com;r++)
        {
            temp_memory[r]=move_strings[r];
        }
        //move_strings=strtok(NULL,";");
        strcpy(word, temp_memory);

        printf("%s generic string after comment\n",word);
        list=g_list_append(list, word);*/
        //move_strings =strtok(NULL,"\n");
/*
        

        continue;
        
    }*/
    else if((move_strings[0] =='{')||(move_strings[0]=='['))
    {
        b=1;
        continue;
    }
    else if(b==1)
    {
        
        if((move_strings[strlen(move_strings)-1]=='}')||(move_strings[strlen(move_strings)-1]==']'))
        b=0;
        continue;
    }

    strcpy(word, move_strings);
    printf("%s generic string\n",word);
    
    list=g_list_append(list, word);
    
}

free(temp_fen);

int len = g_list_length(list);
printf("len %d\n",len);
int k=0;

list=list->next;
game_state current_state;
init_state(&current_state);
/*
for(int j=0;j<len-3;j++)
{   
    

    char white_figures[] = {'R','N','B','Q','K','P'};
    char black_figures[] = {'r','n','b','q','k','p'};
    char figures[6];
    k++;
    switch(k)
    {
        case 0:
        continue;
        break;
        case 1:
        sprintf(figures,"%s",white_figures);
        break;
        case 2:
        sprintf(figures,"%s",black_figures);
        k=0;
        break;

    }
    

    char* current_data= list->data;


    char dragged_piece;
    int from_row=-1, from_col=-1, to_row=-1, to_col=-1;

    int len = strlen(current_data);
    state.side_to_move=1;
    char first_letter = current_data[0];
    if(strchr(figures,first_letter))
    {
                
                if(strrchr(current_data,'x')!=NULL)
                {
                    state.fifty_moves_counter=0;

                    to_row = current_data[3]-97;
                    to_col = current_data[2]-49;

                    if(strlen(current_data)==5)
                    {
                        if(current_data[1]<97)
                        from_row = current_data[1];
                        else
                        from_col = current_data[1];
                        

                        to_row = current_data[4]-97;
                        to_col = current_data[3]-49;
                    }
                }
                else
                {
                    
                    state.fifty_moves_counter++;

                    to_row = current_data[2]-97;
                    to_col = current_data[1]-49;

                    if(strlen(current_data)==4)
                    {
                        if(current_data[1]<97)
                        from_row = current_data[1];
                        else
                        from_col = current_data[1];

                        to_row = current_data[3]-97;
                        to_col = current_data[2]-49;
                    }
                }
                dragged_piece=first_letter;
                if(first_letter==figures[0])
                {
                    if(from_row<0&&from_col<0)
                    {
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[i][to_col]==figures[0])
                            {
                                from_row=i;
                                from_col = to_col;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[0];
                                
                                break;

                            }
                        }

                        for(int i=0; i<8; i++)
                        {
                            if(state.field[to_row][i]==figures[0])
                            {
                                from_col=i;
                                from_row = to_row;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[0];
                                
                                break;

                            }
                        }
                    }
                    else if(from_row<0)
                    {
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[i][from_col]==figures[0])
                            {
                                from_row=i;
                                
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[0];
                                
                                break;

                            }
                        }


                    }
                    else if(from_col<0)
                    {
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[from_row][i]==figures[0])
                            {
                                from_col=i;
                                
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[0];
                                
                                break;

                            }
                            }
                    }
                }
                    

                    if(first_letter==figures[1])
                    {
                    if(from_row<0&&from_col<0)
                    {
                        int ab[] = {2,1};
                        int cd[] = {-2,-1};
                        int t=0;
                        for(int i=0; i<8; i++)
                        {
                            swap(&ab,0,1);
                            swap(&cd,0,1);
                            t++;
                            if(t==2)
                            {
                                ab[0]*=-1;
                                ab[1]*=-1;
                                cd[0]*=-1;
                                cd[1]*=-1;
                            }
                            int a = to_row+ab[0], b =to_col+cd[0];
                            if(a>-1&&b>-1&&a<8&&b<8)
                            if(state.field[a][b]==figures[1])
                            {
                                from_row=to_row+ab[0];
                                from_col = to_col+cd[0];
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[1];
                                
                                break;

                            }
                        }
                    }
                    else if(from_row<0)
                    {
                        int ab[] = {2,1};
                        int t=0;
                        for(int i=0; i<8; i++)
                        {
                            swap(&ab,0,1);
                            t++;
                            if(t==2)
                            {
                                ab[0]*=-1;
                                ab[1]*=-1;
                            }
                            int a = to_row+ab[0];
                            if(a>-1&&a<8)
                            if(state.field[a][from_col]==figures[1])
                            {
                                from_row=to_row+ab[0];
                                
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[1];
                                
                                break;

                            }
                        }


                    }
                    else if(from_col<0)
                    {
                        
                        int cd[] = {-2,-1};
                        int t=0;
                        for(int i=0; i<8; i++)
                        {
                            swap(&cd,0,1);
                            t++;
                            if(t==2)
                            {
                                cd[0]*=-1;
                                cd[1]*=-1;
                            }
                            int b =to_col+cd[0];
                            if(b>-1&&b<8)
                            if(state.field[from_row][b]==figures[1])
                            {
                                from_col=to_col+cd[0];
                                
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[1];
                                
                                break;

                            }
                        }
                    }
                    }

                    if(first_letter==figures[2])
                    {
                    if(from_row<0&&from_col<0)
                    {
                        int a[] = {-1,-2,-3,-4};
                        int b[] = {-1,-2,-3,-4};
                        
                        for(int i=0; i<4; i++)
                        {
                            switch(i)
                            {
                                case 1:
                                min_el(&b,3);
                                break;
                                case 2:
                                min_el(&a,3);
                                min_el(&b,3);
                                break;
                                case 3:
                                min_el(&b,3);
                                break;
                            }
                            
                            for(int j=0; j<4;j++)
                            {
                                int c = to_row+a[j];
                                int d = to_col+b[j];
                                if(c>-1&&d>-1&&c<8&&d<8)
                                if(state.field[c][d]==figures[2])
                                {
                                    from_row = c;
                                    from_col = d;
                                    state.field[from_row][from_col]='-';
                                    state.field[to_row][to_col]=figures[2];
                                }
                            }
                            
                        }
                    }
                    else if(from_row<0)
                    {
                        
                        
                        int b[] = {-1,-2,-3};
                        
                        for(int i=0; i<4; i++)
                        {
                            switch(i)
                            {
                                case 1:
                                min_el(&b,3);
                                break;
                                case 2:
                                
                                min_el(&b,3);
                                break;
                                case 3:
                                min_el(&b,3);
                                break;
                            }
                            
                            for(int j=0; j<3;j++)
                            {
                                
                                int d = to_col+b[j];
                                if(d>-1&&d<8)
                                if(state.field[from_row][d]==figures[2])
                                {
                                    
                                    from_col = d;
                                    state.field[from_row][from_col]='-';
                                    state.field[to_row][to_col]=figures[2];
                                }
                            }
                            
                        }


                    }
                    else if(from_col<0)
                    {
                        int a[] = {-1,-2,-3,-4};
                        
                        
                        for(int i=0; i<4; i++)
                        {
                            
                            if(i==2)
                            min_el(&a,3);
                            for(int j=0; j<4;j++)
                            {
                                int d = to_col+a[j];
                                if(d>-1&&d<8)
                                if(state.field[from_row][d]==figures[2])
                                {

                                    from_col = d;
                                    state.field[from_row][from_col]='-';
                                    state.field[to_row][to_col]=figures[2];
                                }
                            }
                            
                        }
                    }
                    }

                    
                    if(first_letter==figures[3])
                    {
                        int a[] = {-1,-2,-3,-4};
                        int b[] = {-1,-2,-3,-4};
                        int t=0;
                        for(int i=0; i<8; i++)
                        {
                            switch(t)
                            {
                                case 2:
                                min_el(&a,4);
                                break;
                                case 4:
                                min_el(&b,4);
                                break;
                                case 6:
                                min_el(&a,4);
                                break;
                            }
                            if(t%2==0)
                            for(int j=0; j<4;j++)
                            {
                                int c = to_row+a[j];
                                int d = to_col+b[j];
                                if(c>-1&&d>-1&&c<8&&d<8)
                                if(state.field[c][d]==figures[3])
                                {
                                    from_row = c;
                                    from_col = d;
                                    state.field[from_row][from_col]='-';
                                    state.field[to_row][to_col]=figures[3];
                                }
                            }
                            
                            t++;
                        }
                        if(from_col<0|from_row<0)
                        for(int i=0; i<8; i++)
                        {
                                    if(state.field[i][to_col]==figures[3])
                                    {
                                    from_row=i;
                                    from_col = to_col;
                                    state.field[from_row][from_col]='-';
                                    state.field[to_row][to_col]=figures[3];
                                
                                    break;

                                    }
                        }
                        if(from_col<0||from_row<0)
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[to_row][i]==figures[3])
                            {
                                from_col=i;
                                from_row = to_row;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[3];
                                
                                break;

                            }
                        }
                        

                    }

                    if(first_letter==figures[4])
                    {
                    
                    
                        int ab[] = {0,-1,1,-1,1,0,1,1,-1,1,-1,1,-1,0,-1,-1};
                        
                        for(int i=0; i<15; i+=2)
                        {
                            
                            int a = to_row+ab[i], b =to_col+ab[i+1];
                            if(a>-1&&b>-1&&a<8&&b<8)
                            if(state.field[a][b]==figures[4])
                            {
                                from_row=to_row+a;
                                from_col = to_col+b;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[4];
                                
                                break;

                            }
                        }
                    

                    }
                    
                }
            
            else if(first_letter=='O')
            {
                dragged_piece=figures[4];
                from_col=5;

                int l = strlen(current_data);
                if(l<5)//kingside
                { 
                    
                    to_col=6;
                    if(k==1)
                    {
                        to_row = 7; 
                        
                        from_row=7;
                        
                        
                    }
                    else
                    {
                        to_row = 0; 
                        
                        from_row=0;
                        
                        
                    }
                    

                    state.field[from_row][7] = '-';
                    state.field[from_row][from_col] = '-';
                    
                    state.field[from_row][5] = figures[0];
                    state.field[from_row][to_col] = figures[4];
                    

                }
                else//queenside
                {
                    to_col=2;
                    if(k==1)
                    {
                        to_row = 7; 
                        
                        from_row=7;
                        
                        
                    }
                    else
                    {
                        to_row = 0; 
                        
                        from_row=0;
                        
                        
                    }

                    state.field[to_row][0] = '-';
                    state.field[to_row][from_col] = '-';

                    state.field[to_row][3] = figures[0];
                    state.field[to_row][2] = figures[4];
                    
                }
                state.fifty_moves_counter++;

            }
            else
            {
                dragged_piece=figures[5];
                if(strrchr(current_data,'=')!=NULL)
                {
                    if(strrchr(current_data,'x')!=NULL)
                    state.fifty_moves_counter=0;
                    else
                    state.fifty_moves_counter++;
                    //pawn promotion

                }
                
                else if(strchr(current_data,'x')!=NULL)//enpassant
                {
                    state.fifty_moves_counter=0;
                    to_row = current_data[3]-97;
                    to_col = current_data[2]-49;
                    
                    if(state.field[to_row][to_col]=='-')
                    {
                        //enpassant code
                        from_col = current_data[0]-49;
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[i][from_col]==figures[5])
                            {
                                from_row=i;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[5];

                                int e=1;
                                if(k==2)
                                e*=-1;

                                state.field[to_row][to_col+e]='-';
                                break;

                            }
                        }
                    }
                    else
                    {
                        from_col = current_data[0]-49;
                        for(int i=0; i<8; i++)
                        {
                            if(state.field[i][from_col]==figures[5])
                            {
                                from_row=i;
                                state.field[from_row][from_col]='-';
                                state.field[to_row][to_col]=figures[5];
                                break;

                            }
                        }
                    }
                }
                
                else
                {
                    to_row = current_data[1]-97;
                    to_col = current_data[0]-49;
                    from_col = to_col;
                    state.fifty_moves_counter++;
                    for(int i=0; i<8; i++)
                    {
                        if(state.field[i][to_col]==figures[5])
                        {
                            from_row=i;
                            state.field[from_row][from_col]='-';
                            state.field[to_row][to_col]=figures[5];
                            break;

                        }
                    }
                }
            }
            

        

    
    

    //next_move(&state, dragged_piece, from_row, from_col, to_row, to_col,0)
    
}*/
g_list_free(list);
}







