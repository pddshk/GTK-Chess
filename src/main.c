#include <gtk/gtk.h>
#include <cairo.h>
#include <librsvg/rsvg.h>
#include "state.h"
#include "board.h"

game_state state;
GtkTargetList* board_target;
GtkGestureDrag* drag_handler;

int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	init_state();
	load_textures();
	GtkBuilder* builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GObject *Board=gtk_builder_get_object(builder, "Board");
	GtkTargetEntry *board_entry = gtk_target_entry_new(
		"GtkDrawingArea",
		GTK_TARGET_SAME_WIDGET,
		0
	);
	board_target=gtk_target_list_new(
		board_entry,
		1
	);
	drag_handler = GTK_GESTURE_DRAG(gtk_gesture_drag_new(GTK_WIDGET(Board)));
	gtk_drag_dest_set (
		GTK_WIDGET(Board),
		GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
		board_entry,
		1,
		GDK_ACTION_MOVE
	);
	// gtk_drag_source_set (
	// 	GTK_WIDGET(Board),
	// 	GDK_BUTTON1_MASK,
	// 	board_target,
	// 	1,
	// 	GDK_ACTION_MOVE
	// );

	gtk_widget_show(GTK_WIDGET(window));
	gtk_widget_add_events(GTK_WIDGET(Board), GDK_POINTER_MOTION_MASK);
	g_signal_connect(Board, "draw", G_CALLBACK(draw_board), NULL);
	g_signal_connect(Board, "motion-notify-event", G_CALLBACK(drag_start), NULL);
	g_signal_connect(Board, "drag-motion", G_CALLBACK(drag_motion), NULL);
	g_signal_connect(Board, "drag-failed", G_CALLBACK(drag_failed), NULL);
	g_signal_connect(Board, "drag-drop", G_CALLBACK(drag_drop), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();
    return 0;
}
