#include <gtk/gtk.h>
#include <gtk/gtkframe.h>
#include <goocanvas.h>
#include "board.h"

int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	GtkBuilder* builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW (window), 1600, 900);
    GObject *BoardHolder=gtk_builder_get_object(builder, "BoardHolder");
    GtkWidget *Board=goo_canvas_new();
    gtk_box_pack_start(GTK_BOX (BoardHolder), Board, TRUE, TRUE, 0);
    gtk_widget_show(GTK_WIDGET (Board));

    gtk_widget_show(GTK_WIDGET(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(Board, "size_allocate", G_CALLBACK(resize_board), NULL);

    init_board(GOO_CANVAS(Board));

    gtk_main();
    return 0;
}
