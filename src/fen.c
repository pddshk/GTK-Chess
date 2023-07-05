#include "fen.h"

#include "state_tree.h"
#include "state.h"
#include "notation.h"

extern state_tree tree;

void paste_FEN(
    __attribute_maybe_unused__ GtkButton* self, 
    GtkWidget* Board)
{
    GdkDisplay *display = gdk_display_get_default();
    GtkClipboard *clipboard = gtk_clipboard_get_default(display);
    gchar* fen = gtk_clipboard_wait_for_text(clipboard);
    game_state *state = FEN_to_game_state(fen);
    if (fen)
        g_free(fen);
    if (state) {
        int state_validation = validate_state(state);
        if (state_validation == STATE_OK){
            destroy_tree(&tree);
            init_tree(&tree, state);
            gtk_widget_queue_draw(Board);
            show_state(tree.root,0);
        } else {
            puts("FEN is valid, but position is not. Issues are:");
            if (state_validation & PAWN_ON_END_ROW)
                puts("Pawn on 1th or 8th horizontal");
            if (state_validation & TOO_MANY_PIECES)
                puts("Too many pieces");
            if (state_validation & TOO_MANY_KINGS)
                puts("Too many kings");
            if (state_validation & NO_MOVES_POSSIBLE)
                puts("No moves possible");
            if (state_validation & NO_KING)
                puts("No king");
        }
        free(state);
    }
}

static game_state* FEN_to_game_state(const gchar* fen)
{
    if (!fen) {
        (void)fprintf(stderr,"FEN parsing error: clipboard is empty or its content cannot be converted to text\n");
        return NULL;
    }
    game_state res;
    if (
        read_field(&res, &fen)          &&
        read_side_to_move(&res, &fen)   &&
        read_castlings(&res, &fen)      &&
        read_enpassant(&res, &fen)      &&
        read_move_counters(&res, &fen)
    ){
        game_state *result = malloc(sizeof(game_state));
        *result = res;
        return result;
    } else {
        return NULL;
    }
    // cppcheck-suppress uninitdata 
}

static int read_field(game_state* state, const char** fen)
{
#ifdef DEBUG_FEN
    puts(*fen);
#endif
    char *flat_field = state->field[0];
    size_t i_field = 0;
    while (i_field < 8*9-1) {
        // printf("Reading %c, %d\n", *i_fen, i_field);
        switch (**fen)
        {
        case 'q': case 'Q': case 'k': case 'K': case 'n': case 'N':
        case 'b': case 'B': case 'r': case 'R': case 'p': case 'P':
            flat_field[i_field] = **fen;
            i_field++;
            break;
        case '/':
            flat_field[i_field] = '\0';
            i_field++;
            break;
        default:
            if ('1' <= **fen && **fen <= '8') {
                int nempty = **fen - '0';
                for (int i = 0; i < nempty; ++i)
                    flat_field[i_field++] = '-';
            } else {
                (void)fprintf(stderr, "FEN parsing error while reading board: unknown character %c\n", **fen);
                return FALSE;    
            }
        }
        *fen += 1;
    }
    if (**fen != ' '){ 
        (void)fprintf(stderr, "FEN parsing error while reading board: no whitespace delimiter after board\n");
        return FALSE; 
    } 
    *fen+=1;
    return TRUE;
}

static int read_side_to_move(game_state* state, const char** fen)
{
#ifdef DEBUG_FEN
    puts(*fen);
#endif
    if (**fen == 'w')
        state->side_to_move = WHITE;
    else if (**fen == 'b')
        state->side_to_move = BLACK;
    else {
        (void)fprintf(stderr, "FEN parsing error while reading side_to_move: unknown characted %c\n", **fen);
        return FALSE;
    }
    *fen+=1;
    if (**fen != ' ') {
        (void)fprintf(stderr, "FEN parsing error while reading castlings: no whitespace separator\n");
        return FALSE;
    } else {
        *fen+=1;
        return TRUE;
    }
    return TRUE;
}

// read castlings (TODO: think of X-FEN)
static int read_castlings(game_state *state, const char** fen)
{
#ifdef DEBUG_FEN
    puts(*fen);
#endif
    const char *begin = *fen;
    clear_castlings(state);
    int any = FALSE;
    for (int i = 0; **fen != ' ' && i<5; i++, *fen+=1){
        int index = castling_index(**fen);
        if (index != -1 && !state->castlings[index]) {
            state->castlings[index] = TRUE;
            any = TRUE;
        } else if (**fen == '-' && !any){
            any = TRUE;
            i = 4;
            continue;
        } else {
            (void)fprintf(stderr, "FEN parsing error while reading castlings: corrupted string %.*s\n", (int)(*fen - begin + 1), begin);
            return FALSE;
        }
    }
    if (**fen != ' ') {
        (void)fprintf(stderr, "FEN parsing error while reading castlings: no whitespace separator\n");
        return FALSE;
    } else {
        *fen+=1;
        return TRUE;
    }
}

static int read_enpassant(game_state* state, const char** fen)
{
#ifdef DEBUG_FEN
    puts(*fen);
#endif
    const char* begin = *fen;
    if (**fen == '-'){
        state->enpassant_col = state->enpassant_row = -1;
        *fen+=1;
    } else if ('a' <= **fen && **fen <= 'h') {
        state->enpassant_col = **fen - 'a';
        *fen+=1;
        if ('1' <= **fen && **fen <= '8'){
            state->enpassant_row = **fen - '1';
            *fen+=1;
        } else {
            (void)fprintf(stderr, "FEN parsing error while reading enpassant: corrupted string %.*s\n", (int)(*fen - begin + 1), begin);
            return FALSE;
        }
    }
    if (**fen != ' ') {
        (void)fprintf(stderr, "FEN parsing error while reading enpassant: no whitespace separator\n");
        return FALSE;
    } else {
        *fen+=1;
        return TRUE;
    }
}

static int read_move_counters(game_state* state, const char **fen)
{
#ifdef DEBUG_FEN
    puts(*fen);
#endif
    int ret = sscanf(*fen, "%d%d", &state->fifty_moves_counter, &state->move_counter);
    if (ret != 2) {
        (void)fprintf(stderr,"FEN parsing error while reading move counters: corrupted string %s\n", *fen);
        return FALSE;
    }
    if (state->fifty_moves_counter < 0) {
        (void)fprintf(stderr,"FEN parsing error while reading move counters: 50 move counter is < 0\n");
        return FALSE;
    }
    if (state->move_counter < 0) {
        (void)fprintf(stderr,"FEN parsing error while reading move counters: move counter is < 0\n");
        return FALSE;
    }
    return TRUE;
}

void copy_FEN(
    __attribute_maybe_unused__ GtkButton* button, 
    __attribute_maybe_unused__ gpointer data)
{
    GdkDisplay *display = gdk_display_get_default();
    GtkClipboard *clipboard = gtk_clipboard_get_default(display);
    char fen[128] = "";
    game_state_to_FEN(&tree.current->state, fen);
#ifdef DEBUG_FEN
    puts(fen);
#endif
    gtk_clipboard_set_text(clipboard, fen, -1);
}

void game_state_to_FEN(const game_state* state, char* fen)
{
    write_board(state, &fen);
    write_side_to_move(state, &fen);
    write_castlings(state, &fen);
    write_enpassant(state, &fen);
    write_counters(state, &fen);
}

static void write_board(const game_state* state, char** fen)
{
    for (int i = 0; i < 8; ++i) {
        int nskips = 0;
        for(int j = 0; j < 8; ++j){
            if (state->field[i][j] == '-'){
                nskips++;
            } else {
                if (nskips != 0){
                    **fen = nskips + '0';
                    *fen+=1;
                    nskips = 0;
                }
                **fen = state->field[i][j];
                *fen+=1;
            }
        }
        if (nskips != 0) {
            **fen = nskips + '0';
            *fen+=1;
        }
        **fen = '/';
        *fen+=1;
    }
    *(*fen-1) = ' '; // delete trailing '/'
}

static void write_side_to_move(const game_state* state, char** fen)
{
    if (state->side_to_move == WHITE)
        **fen = 'w';
    else
        **fen = 'b';
    *fen+=1;
    **fen = ' ';
    *fen+=1;
}

static void write_castlings(const game_state* state, char** fen)
{
    char *castlings = *fen;
    if (state->castlings[1]){
        **fen = 'K';
        *fen+=1;
    }
    if (state->castlings[0]){
        **fen = 'Q';
        *fen+=1;
    }
    if (state->castlings[3]){
        **fen = 'k';
        *fen+=1;
    }
    if (state->castlings[2]){
        **fen = 'q';
        *fen+=1;
    }
    if (*fen == castlings){ // if none of above
        **fen = '-';
        *fen+=1;
    }
    **fen = ' ';
    *fen+=1;
}

static void write_enpassant(const game_state* state, char** fen)
{
    if (state->enpassant_col == -1) {
        **fen = '-';
        *fen+=1;
    } else {
        get_field_notation(
            state->enpassant_row,
            state->enpassant_col,
            *fen
        );
        *fen += 2;
    }
    **fen = ' ';
    *fen+=1;
}

static void write_counters(const game_state* state, char** fen)
{
    (void)sprintf(*fen, "%d %d", state->fifty_moves_counter, state->move_counter);
}
