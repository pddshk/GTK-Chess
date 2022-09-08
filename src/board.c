#include "board.h"
#include <math.h>

const double border_perc=0.03;

void calc_cell_size(int col, int row, double cell_size, gdouble* x, gdouble* y)
{
	*x = col * cell_size;
	*y = row * cell_size;
}

gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
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
			cairo_set_source_rgb(cr, 0,0,0);
		else
			cairo_set_source_rgb(cr, 1,1,1);
		cairo_rectangle(
			cr,
			x_offset + *x, y_offset + *y,
			cell_size, cell_size
		);
		cairo_fill(cr);
	}
	calc_cell_size(0, 0, cell_size, x, y);
	RsvgRectangle piece_holder;
	piece_holder.x = x_offset + *x;
	piece_holder.y = y_offset + *y;
	piece_holder.width = piece_holder.height = cell_size;
	rsvg_handle_render_document(
		image,
		cr,
		&piece_holder,
		NULL
	);
	g_free(x); g_free(y);
	return FALSE;
}
