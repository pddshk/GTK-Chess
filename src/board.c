#include "board.h"
#include <math.h>
#include <string.h>

enum {
	None,
	WKing, WQueen, WRook, WBishop, WKnight, WPawn,
	BKing, BQueen, BRook, BBishop, BKnight, BPawn,
	WPQueen, WPRook, WPBishop, WPKnight,
    BPQueen, BPRook, BPBishop, BPKnight
};

enum { Npieces = BPKnight };

// pieces and board textures
RsvgHandle *Pieces[Npieces];
RsvgHandle *BoardImage;

void init_textures(){
	for (size_t i = 0; i < Npieces; i++)
		Pieces[i] = NULL;
	BoardImage = NULL;
}

// global state of drag action
// used to render dragged_piece
// following mouse pointer
// and for remembering starting position
// of drag action
// struct {
// 	int status = 0;
// 	char piece = 0;
// 	int row_start, col_start;
// 	int pos_x, pos_y;
// }
char dragged_piece = 0;
int drag_row_start, drag_col_start;
int drag_pos_x, drag_pos_y;
int drag_status = 0;

// TODO highlight king when threatened
int king_threatened_row = -1, king_threatened_col = -1;

// border margin in percent
const double border_perc=0.05;

// calculates size of board and it cells
// used to redraw board
void calc_size(GtkWidget* Board,
	gdouble *wmargin, gdouble *hmargin,
	gdouble *board_size,
	gdouble *cell_size,
	gdouble *w_offset, gdouble *h_offset
)
{
	guint width = gtk_widget_get_allocated_width(Board),
		  height = gtk_widget_get_allocated_height(Board);

	gdouble minimum=fmin(width, height);
	if (minimum == width) {
		*hmargin = (height - 0.96*width) / 2.;
		*wmargin = 0.02*width;
	} else {
		*wmargin = (width - 0.96*height) / 2.;
		*hmargin = 0.02*height;
	}
	*board_size = width - 2*(*wmargin);
	gdouble border_size = border_perc * (*board_size);
	*cell_size = (*board_size - 2*border_size)/8;
	*w_offset = *wmargin + border_size;
	*h_offset = *hmargin + border_size;
}

void load_textures(const char* pack)
{
	const char* const names[] = {
		"WKing", "WQueen", "WRook", "WBishop", "WKnight", "WPawn",
		"BKing", "BQueen", "BRook", "BBishop", "BKnight", "BPawn",
		"WPQueen", "WPRook", "WPBishop", "WPKnight",
    	"BPQueen", "BPRook", "BPBishop", "BPKnight"
	};
	for (size_t i = 1; i < Npieces; i++)
	{
		char path[32] = "src/textures/";
		strcat(path, pack);
		strcat(path, "/");
		strcat(path, names[i-1]);
		strcat(path, ".svg");
		if (Pieces[i])
			g_object_unref(Pieces[i]);
		Pieces[i] = rsvg_handle_new_from_file(path, NULL);
	}
	if (BoardImage)
		g_object_unref(BoardImage);
	char path[32] = "src/textures/";
	strcat(path, pack);
	strcat(path, "/Board.svg");
	BoardImage = rsvg_handle_new_from_file(path, NULL);
}

// resolves piece texture
int resolve_piece(char piece)
{
	switch (piece) {
		case 'k': return BKing;
		case 'K': return WKing;
		case 'q': return BQueen;
		case 'Q': return WQueen;
		case 'r': return BRook;
		case 'R': return WRook;
		case 'b': return BBishop;
		case 'B': return WBishop;
		case 'n': return BKnight;
		case 'N': return WKnight;
		case 'p': return BPawn;
		case 'P': return WPawn;
		default: return 0;
	}
}

// promoted pieces textures
int resolve_promoted_piece(char piece)
{
	switch (piece) {
		case 'q': return BPQueen;
		case 'Q': return WPQueen;
		case 'r': return BPRook;
		case 'R': return WPRook;
		case 'b': return BPBishop;
		case 'B': return WPBishop;
		case 'n': return BPKnight;
		case 'N': return WPKnight;
		default: return 0; // if there's no piece
	}
}

//to call every time any coordinate in taken from frontend
int flip_resolve(int coord) {
	if (flipped) {
		coord = 7 - coord;
	}
	return coord;
}

gboolean draw_board(GtkWidget *Board, cairo_t *cr, __attribute_maybe_unused__ gpointer data)
{
	gdouble hmargin, wmargin, board_size, cell_size, w_offset, h_offset;

	calc_size(Board,
		&wmargin, &hmargin,
		&board_size,
		&cell_size,
		&w_offset, &h_offset
	);

	// thick border board
	cairo_set_source_rgb(cr, 0.05, 0.2, 0.15);
	cairo_rectangle(
		cr,
		wmargin, hmargin, board_size, board_size
	);
	cairo_fill(cr);

	// redner board
	RsvgRectangle board_holder;
	board_holder.x = w_offset;
	board_holder.y = h_offset;
	board_holder.width = board_holder.height = 8*cell_size;
	rsvg_handle_render_document(
		BoardImage,
		cr,
		&board_holder,
		NULL
	);

	// when pawn is to be promoted
	int q_row=-1, r_row=-1, b_row=-1, n_row=-1;
	char q,r,b,n;
	int dir = 1;
	int _pawn_promotion_col = pawn_promotion_col,
		_pawn_promotion_row = pawn_promotion_row;
	//resolve_coord(&tree.current->field, &_pawn_promotion_row, &_pawn_promotion_col);
	switch (pawn_promotion) {
		case 'P':
			q_row=_pawn_promotion_row;
			r_row=_pawn_promotion_row+dir;
			b_row=_pawn_promotion_row+2*dir;
			n_row=_pawn_promotion_row+3*dir;
			q='Q';r='R';b='B';n='N';
			break;
		case 'p':
			q_row=_pawn_promotion_row;
			r_row=_pawn_promotion_row-dir;
			b_row=_pawn_promotion_row-2*dir;
			n_row=_pawn_promotion_row-3*dir;
			q='q';r='r';b='b';n='n';
			break;
	}
	// redner pieces
	for (int row=0; row<8; row++) for (int col=0; col<8; col++) {
		int current_piece=0;
		// if pawn is to be promoted
		if (col == _pawn_promotion_col) {
			if (row == q_row)
				current_piece = resolve_promoted_piece(q);
			else if (row == r_row)
				current_piece = resolve_promoted_piece(r);
			else if (row == b_row)
				current_piece = resolve_promoted_piece(b);
			else if (row == n_row)
				current_piece = resolve_promoted_piece(n);
			else
				current_piece=resolve_piece(get_field(&tree.current->state, row, col));
		} else
			current_piece=resolve_piece(get_field(&tree.current->state, row, col));
		if (current_piece){ // render it
			gdouble x = flip_resolve(col) * cell_size, y = flip_resolve(row) * cell_size;
			RsvgRectangle piece_holder;
			piece_holder.x = w_offset + x;
			piece_holder.y = h_offset + y;
			piece_holder.width = piece_holder.height = cell_size;
			rsvg_handle_render_document(
				Pieces[current_piece],
				cr,
				&piece_holder,
				NULL
			);
		}
	}
	if (drag_status){ // render piece following current cursor position
		int current_piece=resolve_piece(dragged_piece);
		if(current_piece){
			RsvgRectangle piece_holder;
			piece_holder.x = drag_pos_x - cell_size/2;
			piece_holder.y = drag_pos_y - cell_size/2;
			piece_holder.width = piece_holder.height = cell_size;
			rsvg_handle_render_document(
				Pieces[current_piece],
				cr,
				&piece_holder,
				NULL
			);
		}
	}
	return FALSE;
}

void
drag_begin (
  GtkWidget* widget,
  GdkDragContext* context,
  gpointer data
)
{
	// parse passed data. Emtpy drag icon is to be set instead of default one
	GdkPixbuf *empty_icon = data;

	gdouble hmargin, wmargin, board_size, cell_size, w_offset, h_offset;
	calc_size(widget,
		&wmargin, &hmargin,
		&board_size,
		&cell_size,
		&w_offset, &h_offset
	);

	GdkWindow* window = gtk_widget_get_window(widget);
	GdkDevice* device = gdk_drag_context_get_device (context);
	// get starting drag position
	int start_x, start_y;
	gdk_window_get_device_position (
		window,
		device,
		&start_x,
		&start_y,
		NULL
	);
	drag_col_start = flip_resolve((int)((start_x - w_offset) / cell_size));
	drag_row_start = flip_resolve((int)((start_y - h_offset) / cell_size));
	dragged_piece = get_field(&tree.current->state, drag_row_start, drag_col_start);
	// check if piece to be moved is of valid side
	const char* piece_set = tree.current->state.side_to_move ? "KQRBNP" : "kqrbnp";
	if (pawn_promotion == '-' && strchr(piece_set, dragged_piece)){
		// remove dragged piece from state to prevent drawing it
		set_field(&tree.current->state, drag_row_start, drag_col_start, '-');
		gtk_drag_set_icon_pixbuf(context, empty_icon, 0, 0);
		drag_status = 1;
	} else {
		drag_status = 0;
	}
}

gboolean
drag_motion (
  GtkWidget* widget,
  __attribute_maybe_unused__ GdkDragContext* context,
  gint x,
  gint y,
  __attribute_maybe_unused__ guint time,
  __attribute_maybe_unused__ gpointer user_data
)
{
	// update current drag position and redraw board
	if (drag_status){
		drag_pos_x = x;
		drag_pos_y = y;
		gtk_widget_queue_draw(widget);
	}
	return TRUE;
}

gboolean
drag_failed (
  GtkWidget* self,
  __attribute_maybe_unused__ GdkDragContext* context,
  __attribute_maybe_unused__ GtkDragResult result,
  __attribute_maybe_unused__ gpointer user_data
)
{
	cancel_drag(&tree.current->state, dragged_piece, drag_row_start, drag_col_start);
	drag_pos_x = drag_pos_y = -1;
	drag_status = 0;
	gtk_widget_queue_draw(self);
	return TRUE;
}

gboolean
drag_drop (
  GtkWidget* widget,
  __attribute_maybe_unused__ GdkDragContext* context,
  gint x,
  gint y,
  __attribute_maybe_unused__ guint time,
  gpointer data
)
{

	gdouble hmargin, wmargin, board_size, cell_size, w_offset, h_offset;
	calc_size(widget,
		&wmargin, &hmargin,
		&board_size,
		&cell_size,
		&w_offset, &h_offset
	);

	int col = flip_resolve((int)((x - w_offset) / cell_size)), row = flip_resolve((int)((y - h_offset) / cell_size));
	int from_row = drag_row_start, from_col = drag_col_start, to_row = row, to_col = col;
	// resolves coordinates in case board is flipped
	//resolve_coord(&tree.current->field, &from_row, &from_col);
	//resolve_coord(&tree.current->field, &to_row, &to_col);
	// chek if there was move and move is valid
	if (drag_status && is_valid_move(&tree.current->state, dragged_piece, from_row, from_col, to_row, to_col))
	{
		if (is_pawn_promotion(dragged_piece, to_row)){
			pawn_promotion = dragged_piece;
	        pawn_promotion_row = to_row;
	        pawn_promotion_col = to_col;
		} 
		else 
		{
			cancel_drag(&tree.current->state, dragged_piece, drag_row_start, drag_col_start);
			//return piece and then move to save current state

			next_move(&tree.current->state, dragged_piece, from_row, from_col, to_row, to_col,0);
		}
		
	}
	else
		cancel_drag(&tree.current->state, dragged_piece, drag_row_start, drag_col_start);

	gtk_widget_queue_draw(widget);
	drag_pos_x = drag_pos_y = -1;
	drag_status = 0;
	drag_col_start = drag_row_start = 0;
	// print_state(&state);
	// parse incoming data
	GtkWidget **dialogs = data; // mate stalemate and im dialogs
	if (is_mate(&tree.current->state))
        gtk_dialog_run(GTK_DIALOG (dialogs[0]));
	else if (is_stalemate(&tree.current->state))
        gtk_dialog_run(GTK_DIALOG (dialogs[1]));
	else if (insufficient_material(&tree.current->state))
		gtk_dialog_run(GTK_DIALOG(dialogs[2]));
	return TRUE;

}

// TODO: write entering move with 2 clicks
// now is used to choose promoted piece only
gboolean
board_clicked (
  GtkWidget* widget,
  GdkEventButton *event,
  __attribute_maybe_unused__ gpointer user_data
)
{
	if (event->type == GDK_BUTTON_RELEASE && pawn_promotion != '-'){
		gdouble hmargin, wmargin, board_size, cell_size, w_offset, h_offset;

		calc_size(widget,
			&wmargin, &hmargin,
			&board_size,
			&cell_size,
			&w_offset, &h_offset
		);

		int col = flip_resolve((int)((event->x - w_offset) / cell_size)),
			row = flip_resolve((int)((event->y - h_offset) / cell_size));
		//resolve_coord(&tree.current->field, &row, &col);
		if (col != pawn_promotion_col) return TRUE;
		if ((pawn_promotion == 'P' && tree.current->state.side_to_move && row < 4) ||
			(pawn_promotion == 'p' && !(tree.current->state.side_to_move) && row > 3)) {
			
			cancel_drag(&tree.current->state, dragged_piece, drag_row_start, drag_col_start);
			//return piece and then move to save current state

			next_move(
				&tree.current->state,
				'P',
				drag_row_start, drag_col_start,
				pawn_promotion_row, pawn_promotion_col,
				resolve_promotion(row)
			);
			pawn_promotion = '-';
			pawn_promotion_row = pawn_promotion_col = -1;
			gtk_widget_queue_draw(widget);
		}
	}
	return TRUE;
}
