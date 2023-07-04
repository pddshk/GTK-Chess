#include "fen.h"

#include "state_tree.h"
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
    for (int i = 0; i < 4; i++) // max 4 symbols KQkq
    {
        switch (*i_fen)
        {
        case 'K':
            res.castlings[0] = TRUE; break;
        case 'Q':
            res.castlings[1] = TRUE; break;
        case 'k':
            res.castlings[2] = TRUE; break;
        case 'q':
            res.castlings[3] = TRUE; break;
        case ' ':
            i = 4; break; // end cycle
        case '-':
            if (i == 0)
                i = 4;
            else {
        default:
            raise_error();
            return NULL;
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
    printf("%d, %d\n", res.fifty_moves_counter, res.move_counter);
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

// void FEN_to_state(const char* fen) {
//     char delim[] = " ";
//     game_state newstate;
//     char* placement = strtok((char*)fen, delim);
//     char field_ptr = 0;
//     for(size_t i = 0; i < strlen(placement); field_ptr++) 
//     {
//         if (placement[i] < '0' || placement[i] > '9') {
//             if (placement[i] == '/') {
//                 newstate.field[field_ptr / 9][field_ptr % 9] = '\0';
//             }
//             else {
//                 newstate.field[field_ptr / 9][field_ptr % 9] = placement[i];
//             }
//             i++;
//         }
//         else {
//             placement[i]--;
//             newstate.field[field_ptr / 9][field_ptr % 9] = '-';
//             if (placement[i] == '0') i++;
//         }
//     }
//     char* color = strtok(NULL, delim);
//     if (color[0] == 'w') {
//         newstate.side_to_move = 1;
//     }
//     else {
//         newstate.side_to_move = 0;
//     }
//     char* castling = strtok(NULL, delim);
//     for(size_t i = 0; i < strlen(castling); i++) {
//         if (castling[i] == 'K') {
//             newstate.castlings[1] = 1;
//         }
//         if (castling[i] == 'Q') {
//             newstate.castlings[0] = 1;
//         }
//         if (castling[i] == 'k') {
//             newstate.castlings[3] = 1;
//         }
//         if (castling[i] == 'Q') {
//             newstate.castlings[2] = 1;
//         }
//     }
//     char* enpassant_square = strtok(NULL, delim);
//     newstate.enpassant_col = newstate.enpassant_row = -1;
//     if (isalpha(enpassant_square[0])) {
//         newstate.enpassant_col = enpassant_square[0] - 'a';
//         newstate.enpassant_row = enpassant_square[1] - '0';
//     }
//     char* fiftymoves = strtok(NULL, delim);
//     sscanf(fiftymoves, "%d", &newstate.fifty_moves_counter);
//     char* fullmove = strtok(NULL, delim);
//     sscanf(fullmove, "%d", &newstate.move_counter);
//     newstate.is_active = tree.current->field.is_active;
//     newstate.flipped = tree.current->field.flipped;
//     tree.current->field = newstate;
// }

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
