#include "board.h"
#include <math.h>

void init_board(GooCanvas* canvas, GooCanvasItemModel *rect)
{
	GooCanvasItemModel *root=goo_canvas_group_model_new(NULL, NULL);

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
	rect = goo_canvas_rect_model_new(root,
		wmargin, hmargin, width-2*wmargin, height-2*hmargin,
		"line-width", 10.0,
		"stroke-color", "darkblue",
		"fill-color", "lightblue",
		NULL
	);
	goo_canvas_set_root_item_model(canvas, root);
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
	GooCanvasItemModel *root=goo_canvas_get_root_item_model(canvas);
	GooCanvasItemModel *rect_model=goo_canvas_item_model_get_child(root, 0);
	g_object_set(G_OBJECT(rect_model),
		"x", wmargin,
		"y", hmargin,
		"width", width - 2*wmargin,
		"height", height - 2*hmargin,
		NULL
	);
}
