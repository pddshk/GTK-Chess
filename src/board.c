#include "board.h"
#include <math.h>

RsvgHandle *BKing, *WKing, *BQueen, *WQueen, *BRook, *WRook,
    *BBishop, *WBishop, *BKnight, *WKnight, *BPawn, *WPawn, *Board;

char dragged_piece = 0;
int dragged_x, dragged_y;
int drag_pos_x, drag_pos_y;

const double border_perc=0.05;

void load_textures(/* const char* pack */)
{
	BKing	= rsvg_handle_new_from_file("src/textures/classic/BKing.svg",	NULL);
	WKing	= rsvg_handle_new_from_file("src/textures/classic/WKing.svg", 	NULL);
	BQueen	= rsvg_handle_new_from_file("src/textures/classic/BQueen.svg", 	NULL);
	WQueen	= rsvg_handle_new_from_file("src/textures/classic/WQueen.svg", 	NULL);
	BRook	= rsvg_handle_new_from_file("src/textures/classic/BRook.svg", 	NULL);
	WRook   = rsvg_handle_new_from_file("src/textures/classic/WRook.svg", 	NULL);
	BBishop	= rsvg_handle_new_from_file("src/textures/classic/BBishop.svg",	NULL);
	WBishop	= rsvg_handle_new_from_file("src/textures/classic/WBishop.svg",	NULL);
	BKnight	= rsvg_handle_new_from_file("src/textures/classic/BKnight.svg",	NULL);
	WKnight	= rsvg_handle_new_from_file("src/textures/classic/WKnight.svg",	NULL);
	BPawn	= rsvg_handle_new_from_file("src/textures/classic/BPawn.svg",	NULL);
	WPawn	= rsvg_handle_new_from_file("src/textures/classic/WPawn.svg",	NULL);
	Board	= rsvg_handle_new_from_file("src/textures/classic/Board.svg",	NULL);
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

gboolean draw_board(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	guint width = gtk_widget_get_allocated_width(widget),
		  height = gtk_widget_get_allocated_height (widget);

	gdouble minimum=fmin(width, height);
	gdouble hmargin, wmargin;
	if (minimum == width) {
		hmargin = (height - 0.96*width) / 2.;
		wmargin = 0.02*width;
	} else {
		wmargin = (width - 0.96*height) / 2.;
		hmargin = 0.02*height;
	}
	gdouble board_size=width-2*wmargin;
	gdouble border_size=border_perc * board_size;
	gdouble cell_size=(board_size - 2*border_size)/8;
	gdouble x_offset=wmargin+border_size, y_offset=hmargin+border_size;

	cairo_set_source_rgb(cr, 0.05, 0.2, 0.15);
	cairo_rectangle(
		cr,
		wmargin, hmargin, board_size, board_size
	);
	cairo_fill(cr);

	RsvgRectangle board_holder;
	board_holder.x = x_offset;
	board_holder.y = y_offset;
	board_holder.width = board_holder.height = (board_size - 2*border_size);
	rsvg_handle_render_document(
		Board,
		cr,
		&board_holder,
		NULL
	);
	for (int i = 0; i < 64; i++) {
		int col=i/8, row=i%8;
		RsvgHandle *current_piece=resolve_piece(state.field[row][col]);
		if (current_piece){
			gdouble x = col * cell_size, y = row * cell_size;
			RsvgRectangle piece_holder;
			piece_holder.x = x_offset + x;
			piece_holder.y = y_offset + y;
			piece_holder.width = piece_holder.height = cell_size;
			rsvg_handle_render_document(
				current_piece,
				cr,
				&piece_holder,
				NULL
			);
		}
	}
	if (drag_pos_x != -1){
		RsvgHandle *current_piece=resolve_piece(dragged_piece);
		if(current_piece){
			RsvgRectangle piece_holder;
			piece_holder.x = drag_pos_x - cell_size/2;
			piece_holder.y = drag_pos_y - cell_size/2;
			piece_holder.width = piece_holder.height = cell_size;
			rsvg_handle_render_document(
				current_piece,
				cr,
				&piece_holder,
				NULL
			);
		}
	}
	return FALSE;
}

gboolean
drag_start(GtkWidget *widget,
    GdkEventMotion *event,
    gpointer data)
{
	gdouble start_x, start_y;
	if(!gtk_gesture_drag_get_start_point(
		drag_handler,
		&start_x,
		&start_y
	))
		return FALSE;

	if (gtk_drag_check_threshold(
		widget,
		start_x,
		start_y,
		event->x,
		event->y
	)){
		guint width = gtk_widget_get_allocated_width(widget),
			  height = gtk_widget_get_allocated_height(widget);

		gdouble minimum=fmin(width, height);
		gdouble hmargin, wmargin;
		if (minimum == width) {
			hmargin = (height - 0.96*width) / 2.;
			wmargin = 0.02*width;
		} else {
			wmargin = (width - 0.96*height) / 2.;
			hmargin = 0.02*height;
		}
		gdouble board_size=width-2*wmargin;
		gdouble border_size=border_perc * board_size;
		gdouble cell_size=(board_size - 2*border_size)/8;
		gdouble x_offset=wmargin+border_size, y_offset=hmargin+border_size;
		drag_pos_x = (int) (event->x);
		drag_pos_y = (int) (event->y);
		dragged_x = (int)((event->x - x_offset) / cell_size);
		dragged_y = (int)((event->y - y_offset) / cell_size);
		dragged_piece = state.field[dragged_y][dragged_x];
		if (state.field[dragged_y][dragged_x] != '-'){
			state.field[dragged_y][dragged_x] = '-';
			GdkDragContext *context = gtk_drag_begin_with_coordinates(
				widget,
				board_target,
				GDK_ACTION_MOVE,
				GDK_BUTTON1_MASK,
				(GdkEvent*)event,
				-1,
				-1
			);
			gtk_drag_set_icon_pixbuf (
				context,
				gdk_pixbuf_new (GDK_COLORSPACE_RGB, 0, 8, 1, 1),
				0,
				0
			);
		}
	}
	return TRUE;
}

gboolean
drag_motion (
  GtkWidget* widget,
  GdkDragContext* context,
  gint x,
  gint y,
  guint time,
  gpointer user_data
)
{
	drag_pos_x = x;
	drag_pos_y = y;
	gtk_widget_queue_draw(widget);
	return TRUE;
}

gboolean
drag_failed (
  GtkWidget* self,
  GdkDragContext* context,
  GtkDragResult result,
  gpointer user_data
)
{
	state.field[dragged_y][dragged_x] = dragged_piece;
	dragged_x = dragged_y = 0;
	drag_pos_x = drag_pos_y = -1;
	gtk_widget_queue_draw(self);
	return TRUE;
}

gboolean
drag_drop (
  GtkWidget* widget,
  GdkDragContext* context,
  gint x,
  gint y,
  guint time,
  gpointer user_data
)
{
	guint width = gtk_widget_get_allocated_width(widget),
		  height = gtk_widget_get_allocated_height(widget);

	gdouble minimum=fmin(width, height);
	gdouble hmargin, wmargin;
	if (minimum == width) {
		hmargin = (height - 0.96*width) / 2.;
		wmargin = 0.02*width;
	} else {
		wmargin = (width - 0.96*height) / 2.;
		hmargin = 0.02*height;
	}
	gdouble board_size=width-2*wmargin;
	gdouble border_size=border_perc * board_size;
	gdouble cell_size=(board_size - 2*border_size)/8;
	gdouble x_offset=wmargin+border_size, y_offset=hmargin+border_size;
	int col = (int)((x - x_offset) / cell_size), row = (int)((y - y_offset) / cell_size);

	state.field[row][col] = dragged_piece;
	state.field[dragged_y][dragged_x] = '-';

	gtk_widget_queue_draw_area (
		widget,
		0,
		0,
		width,
		height
	);
	dragged_x = dragged_y = 0;
	drag_pos_x = drag_pos_y = -1;
	return TRUE;
}
