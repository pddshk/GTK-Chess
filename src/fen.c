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
    const gchar* fen = gtk_clipboard_wait_for_text(clipboard);
    game_state *state = FEN_to_game_state(fen);
    if (state) {
        destroy_tree(&tree);
        init_tree(&tree, state);
        free(state);
        gtk_widget_queue_draw(Board);
        show_state(tree.root,0);
    }
}

#define CHECK_DELIMITER(i_fen) \
    if (*i_fen != ' '){ \
        raise_error(); \
        return NULL; \
    } \
    i_fen++;

static game_state* FEN_to_game_state(const gchar* fen)
{
    if (!fen) return NULL;
    game_state res;
    const char *i_fen = fen;
    
    // reading field
    // cppcheck-suppress uninitdata
    char *flat_field = res.field[0];
    size_t i_field = 0;
    while (i_field < 8*9-1) {
        // printf("Reading %c, %d\n", *i_fen, i_field);
        switch (*i_fen)
        {
        case 'q': case 'Q': case 'k': case 'K': case 'n': case 'N':
        case 'b': case 'B': case 'r': case 'R': case 'p': case 'P':
            flat_field[i_field] = *i_fen;
            i_field++;
            i_fen++;
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': {
            int nempty = *i_fen - '0';
            for (int i = 0; i < nempty; ++i) {
                flat_field[i_field++] = '-';
            }
            i_fen++;
            break;
        }
        case '/':
            flat_field[i_field] = '\0';
            i_field++;
            i_fen++;
            break;
        default:
            raise_error();
            return NULL;
        }
    }
    CHECK_DELIMITER(i_fen);

    // read side to move
    switch (*i_fen)
    {
    case 'w':
        res.side_to_move = WHITE; break;
    case 'b':
        res.side_to_move = BLACK; break;
    default:
        raise_error();
        return NULL;
    }
    i_fen++;
    CHECK_DELIMITER(i_fen);

    // read castlings (TODO: think of X-FEN)
    clear_castlings(&res);
    for (int i = 0; i < 4; i++) // max 4 symbols KQkq
    {
        switch (*i_fen)
        {
        case 'Q':
            res.castlings[0] = TRUE; break;
        case 'K':
            res.castlings[1] = TRUE; break;
        case 'q':
            res.castlings[2] = TRUE; break;
        case 'k':
            res.castlings[3] = TRUE; break;
        case '-':
            if (i != 0){
        default:
                raise_error();
                return NULL;
            } else {
                i_fen++;
        case ' ':
                i = 4;
                continue;
        }}
        i_fen++;
    }
    CHECK_DELIMITER(i_fen);

    // read enpassant
    if (*i_fen == '-'){
        res.enpassant_col = res.enpassant_row = -1;
        i_fen++;
    } else if ('a' <= *i_fen && *i_fen <= 'h') {
        res.enpassant_col = *i_fen - 'a';
        i_fen++;
        if ('1' <= *i_fen && *i_fen <= '8'){
            res.enpassant_row = *i_fen - '1';
            i_fen++;
        } else {
            raise_error();
            return NULL;
        }
    }
    CHECK_DELIMITER(i_fen);

    // read 50 moves counter
    int ret = sscanf(i_fen, "%d%d", &res.fifty_moves_counter, &res.move_counter);
    if (ret != 2) {
        raise_error();
        return NULL;
    }
    game_state *result = malloc(sizeof(game_state));
    *result = res;
    return result;
}

inline void raise_error(void)
{
    (void)fprintf(stderr, "Invalid FEN!\n");
}

void copy_FEN(
    __attribute_maybe_unused__ GtkButton* button, 
    __attribute_maybe_unused__ gpointer data)
{
    GdkDisplay *display = gdk_display_get_default();
    GtkClipboard *clipboard = gtk_clipboard_get_default(display);
    char fen[128] = "";
    game_state_to_FEN(&tree.current->state, fen);
    gtk_clipboard_set_text(clipboard, fen, -1);
}

void game_state_to_FEN(const game_state* state, char* fen)
{
    for (int i = 0; i < 8; ++i) {
        int nskips = 0;
        for(int j = 0; j < 8; ++j){
            if (state->field[i][j] == '-'){
                nskips++;
            } else {
                if (nskips != 0){
                    *fen = nskips + '0';
                    fen++;
                    nskips = 0;
                }
                *fen = state->field[i][j];
                fen++;
            }
        }
        if (nskips != 0) {
            *fen = nskips + '0';
            fen++;
        }
        *fen = '/';
        fen++;
    }
    *(fen-1) = ' '; // delete trailing '/'
    if (state->side_to_move == WHITE)
        *fen = 'w';
    else
        *fen = 'b';
    fen++;
    *fen = ' ';
    fen++;
    char *castlings = fen;
    if (state->castlings[1]){
        *fen = 'K';
        fen++;
    }
    if (state->castlings[0]){
        *fen = 'Q';
        fen++;
    }
    if (state->castlings[3]){
        *fen = 'k';
        fen++;
    }
    if (state->castlings[2]){
        *fen = 'q';
        fen++;
    }
    if (fen == castlings){ // if none of above
        *fen = '-';
        fen++;
    }
    *fen = ' ';
    fen++;
    if (state->enpassant_col == -1) {
        *fen = '-';
        fen++;
    } else {
        get_field_notation(
            state->enpassant_row,
            state->enpassant_col,
            fen
        );
        fen += 2;
    }
    *fen = ' ';
    fen++;
    (void)sprintf(fen, "%d %d", state->fifty_moves_counter, state->move_counter);
}

// void get_FEN(__attribute_maybe_unused__ GtkButton* button, gpointer data)
// {
// 	GtkWidget* widget = GTK_WIDGET(data);
// 	GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG (widget));
// 	GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
// 	GtkWidget *grid = children->data;
// 	GList *gchildren = gtk_container_get_children(GTK_CONTAINER(grid));
	
//     GtkEntry* entry = GTK_ENTRY(gchildren->next->data);
//     FEN_to_state(gtk_entry_get_text(entry));
// 	game_state state = tree.current->field;
// 	destroy_tree(&tree);
// 	init_tree(state);
// 	show_state(tree.root, 0);
//     gtk_widget_destroy (widget); // This will close the dialog
// 	//gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
// }
