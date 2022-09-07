#include "board.h"
#include <math.h>

const double border_perc=0.03;

void calc_cell_size(int col, int row, double cell_size, gdouble* x, gdouble* y)
{
	*x = col * cell_size;
	*y = row * cell_size;
}

void init_board(GooCanvas* canvas)
{
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(GTK_WIDGET(canvas), alloc);
	double width=alloc->width, height=alloc->height;
	g_free(alloc);

	double minimum=fmin(width, height);
	goo_canvas_set_bounds(canvas, 0, 0, width, height);
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
	GooCanvasItem *root=goo_canvas_get_root_item(canvas);
	GooCanvasItem *board=goo_canvas_group_new(root, NULL);
	GooCanvasItem *pieces=goo_canvas_group_new(root, NULL);
	GooCanvasItem *field=goo_canvas_group_new(board, NULL);
	GooCanvasItem *cells=goo_canvas_group_new(board, NULL);
	goo_canvas_grid_new(
		field,
		wmargin + border_size, hmargin + border_size,
		(board_size - 2*border_size),
		(board_size - 2*border_size),
		cell_size, cell_size, 0, 0,
		"fill-color", "lightblue",
		NULL
	);
	double x_offset=wmargin+border_size, y_offset=hmargin+border_size;
	gdouble *x=g_new(gdouble, 1), *y=g_new(gdouble, 1);
	for (int i = 0; i < 64; i++) {
		int col=i/8, row=i%8;
		calc_cell_size(col, row, cell_size, x, y);
		goo_canvas_rect_new(
			cells,
			x_offset + *x, y_offset + *y, cell_size, cell_size,
	       	"fill-color", ((col + row) % 2 == 0) ? "white" : "black",
			"stroke-color", "none",
			NULL
		);
	}
	g_free(x); g_free(y);

	return;
}

void resize_board(GtkWidget *widget, gpointer data)
{
	GooCanvas *canvas = GOO_CANVAS(widget);
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(widget, alloc);
	double width=alloc->width, height=alloc->height;
	g_free(alloc);
	double minimum=fmin(width, height);
	goo_canvas_set_bounds(canvas, 0, 0, width, height);
	double hmargin, wmargin;
	if (minimum == width) {
		hmargin = (height - 0.96*width) / 2.;
		wmargin = 0.02*width;
	} else {
		wmargin = (width - 0.96*height) / 2.;
		hmargin = 0.02*height;
	}
	double board_size=width-2*wmargin;
	double border_size=border_perc*board_size;
	double cell_size=(board_size - 2*border_size)/8;
	GooCanvasItem *root=goo_canvas_get_root_item(canvas);
	GooCanvasItem *board=goo_canvas_item_get_child(root, 0);
	GooCanvasItem *pieces=goo_canvas_item_get_child(root, 1);
	GooCanvasItem *field=goo_canvas_item_get_child(board, 0);
	GooCanvasItem *cells=goo_canvas_item_get_child(board, 1);
	GooCanvasItem *frame=goo_canvas_item_get_child(field, 0);
	g_object_set(
		G_OBJECT(frame),
		"x", wmargin + border_size,
		"y", hmargin + border_size,
		"width", (board_size - 2*border_size),
		"height", (board_size - 2*border_size),
		"x_step", cell_size,
		"y_step", cell_size,
		NULL
	);

	double x_offset=wmargin+border_size, y_offset=hmargin+border_size;
	gdouble *x=g_new(gdouble, 1), *y=g_new(gdouble, 1);
	//for (int i = 63; i >= 0; goo_canvas_item_remove_child(cells, i--));
	for (int i = 0; i < 64; i++) {
		int col=i/8, row=i%8;
		calc_cell_size(col, row, cell_size, x, y);
		GooCanvasItem *cell=goo_canvas_item_get_child(cells, i);
		g_object_set(
			G_OBJECT(cell),
			"x", x_offset + *x,
			"y", y_offset + *y,
			"width", cell_size,
			"height", cell_size,
			NULL
		);
	}
	g_free(x); g_free(y);

}
