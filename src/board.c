#include "board.h"
#include <math.h>

RsvgHandle *BKing, *WKing, *BQueen, *WQueen, *BRook, *WRook,
    *BBishop, *WBishop, *BKnight, *WKnight, *BPawn, *WPawn;

const double border_perc=0.03;

void load_textures(/*const char* pack*/)
{
	BKing = rsvg_handle_new_from_file("src/textures/classic/BKing.svg", NULL);
	WKing = rsvg_handle_new_from_file("src/textures/classic/WKing.svg", NULL);
	BQueen = rsvg_handle_new_from_file("src/textures/classic/BQueen.svg", NULL);
	WQueen = rsvg_handle_new_from_file("src/textures/classic/WQueen.svg", NULL);
	BRook = rsvg_handle_new_from_file("src/textures/classic/BRook.svg", NULL);
	WRook = rsvg_handle_new_from_file("src/textures/classic/WRook.svg", NULL);
	BBishop = rsvg_handle_new_from_file("src/textures/classic/BBishop.svg", NULL);
	WBishop = rsvg_handle_new_from_file("src/textures/classic/WBishop.svg", NULL);
	BKnight = rsvg_handle_new_from_file("src/textures/classic/BKnight.svg", NULL);
	WKnight = rsvg_handle_new_from_file("src/textures/classic/WKnight.svg", NULL);
	BPawn = rsvg_handle_new_from_file("src/textures/classic/BPawn.svg", NULL);
	WPawn = rsvg_handle_new_from_file("src/textures/classic/WPawn.svg", NULL);
}

RsvgHandle* resolve_piece(char code)
{
	switch (code) {
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
		default: return NULL;
	}
}

void calc_cell_size(int col, int row, double cell_size, gdouble* x, gdouble* y)
{
	*x = col * cell_size;
	*y = row * cell_size;
}

gboolean
draw_board(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	guint width, height;
	GtkStyleContext *context;
	context = gtk_widget_get_style_context (widget);

	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	double minimum=fmin(width, height);
	double hmargin, wmargin;
	if (minimum == width) {
		hmargin = (height - 0.96*width) / 2.;
		wmargin = 0.02*width;
	} else {
		wmargin = (width - 0.96*height) / 2.;
		hmargin = 0.02*height;
	}
	double board_size=width-2*wmargin;
	double border_size=border_perc * board_size;
	double cell_size=(board_size - 2*border_size)/8;
	double x_offset=wmargin+border_size, y_offset=hmargin+border_size;

	gtk_render_background (context, cr, 0, 0, width, height);
	cairo_set_source_rgb(cr, 0.1, 0.4, 0.3);
	cairo_rectangle(
		cr,
		wmargin, hmargin, board_size, board_size
	);
	cairo_fill(cr);
	gdouble *x=g_new(gdouble, 1), *y=g_new(gdouble, 1);
	for (int i = 0; i < 64; i++) {
		int col=i/8, row=i%8;
		calc_cell_size(col, row, cell_size, x, y);
		if ((col+row)%2)
			cairo_set_source_rgb(cr, 0.4,0.4,0.4);
		else
			cairo_set_source_rgb(cr, 1,1,1);
		cairo_rectangle(
			cr,
			x_offset + *x, y_offset + *y,
			cell_size, cell_size
		);
		cairo_fill(cr);
		RsvgHandle *current_piece=resolve_piece(game_state.field[row][col]);
		if (current_piece){
			RsvgRectangle piece_holder;
			piece_holder.x = x_offset + *x;
			piece_holder.y = y_offset + *y;
			piece_holder.width = piece_holder.height = cell_size;
			rsvg_handle_render_document(
				current_piece,
				cr,
				&piece_holder,
				NULL
			);
		}
	}
	g_free(x); g_free(y);
	return FALSE;
}
