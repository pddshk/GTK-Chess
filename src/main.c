#include <gtk/gtk.h>
#include <cairo.h>
#include <librsvg/rsvg.h>
#include "state.h"
#include "board.h"

board_state game_state;

int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	init_state();
	load_textures();
	GtkBuilder* builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GObject *Board=gtk_builder_get_object(builder, "Board");
    gtk_widget_show(GTK_WIDGET(window));
	g_signal_connect(Board, "draw", G_CALLBACK(draw_board), NULL);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();
    return 0;
}
