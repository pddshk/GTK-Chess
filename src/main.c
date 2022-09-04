#include <gtk/gtk.h>
#include <gtk/gtkframe.h>
#include <goocanvas.h>

void draw_board(GooCanvas*);

void draw_board(GooCanvas* canvas)
{
	
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
    gtk_widget_get_allocation(GTK_WIDGET(canvas), alloc);
	double width=alloc->width, height=alloc->height;
	g_free(alloc);
	
	GooCanvasItem *root;//, *rect_item;
	goo_canvas_set_bounds(canvas, 0, 0, width, height);
	root = goo_canvas_get_root_item(canvas);
	goo_canvas_rect_new (root, 0, 0, width, height,
                                   "line-width", 10.0,
                                   "radius-x", 20.0,
                                   "radius-y", 10.0,
                                   "stroke-color", "yellow",
                                   "fill-color", "red",
                                   NULL);
	return;
}

void resize_board(GtkWidget *widget, gpointer data)
{
	draw_board(GOO_CANVAS(widget));
}

int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	GtkBuilder* builder = gtk_builder_new_from_file("src/window.glade");
	GObject* window = gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW (window), 1600, 900);
    GObject *BoardHolder = gtk_builder_get_object(builder, "BoardHolder");
    GtkWidget *Board = goo_canvas_new();
    //GtkWidget *Board = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX (BoardHolder), Board, TRUE, TRUE, 0);
    //gtk_widget_set_size_request(GTK_WIDGET (Board), 500, 500);
    gtk_widget_show(GTK_WIDGET (Board));
    
    gtk_widget_show(GTK_WIDGET(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(Board, "size_allocate", G_CALLBACK(resize_board), NULL);
    
    draw_board(GOO_CANVAS (Board));
    
    gtk_main();
    return 0;
}
