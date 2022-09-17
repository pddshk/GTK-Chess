#include <gtk/gtk.h>
#include <cairo.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "board.h"

game_state state;
GtkTargetList* board_target;
GtkGestureDrag* drag_handler;
GdkPixbuf *empty_icon;
GtkWidget *mate_dialog, *stalemate_dialog;
int from_engine[2], to_engine[2];
char *engine="bin/stockfish_15";

int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

void init_elements();

int main(int argc, char** argv)
{
	pipe(from_engine);
	pipe(to_engine);
	pid_t pid = fork();
	switch (pid) {
		case -1:
			perror("Fork failed. Try relaunch application\n");
			exit(EXIT_FAILURE);
		case 0:
			while ((dup2(from_engine[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
			while (dup2(to_engine[0], STDIN_FILENO) == -1) {}
			close(from_engine[0]);
			close(from_engine[1]);
			execl(engine, "", NULL);
			break;
		default:
			gtk_init(&argc, &argv);
			init_elements();
			gtk_main();
			break;
	}
    return EXIT_SUCCESS;
}

void init_elements()
{
	init_state(&state);
	load_textures();
	GtkBuilder* builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GObject *Board=gtk_builder_get_object(builder, "Board");
	empty_icon = gdk_pixbuf_new (GDK_COLORSPACE_RGB, 0, 8, 1, 1);
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
	gtk_drag_source_set (
		GTK_WIDGET(Board),
		GDK_BUTTON1_MASK,
		board_entry,
		1,
		GDK_ACTION_MOVE
	);
	gtk_widget_show(GTK_WIDGET(window));
	g_signal_connect(Board, "draw", G_CALLBACK(draw_board), NULL);
	g_signal_connect(Board, "button-release-event", G_CALLBACK(board_clicked), NULL);
	g_signal_connect(Board, "drag-begin", G_CALLBACK(drag_begin), NULL);
	g_signal_connect(Board, "drag-motion", G_CALLBACK(drag_motion), NULL);
	g_signal_connect(Board, "drag-failed", G_CALLBACK(drag_failed), NULL);
	g_signal_connect(Board, "drag-drop", G_CALLBACK(drag_drop), NULL);
	mate_dialog = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Mate"
	);
	g_signal_connect(mate_dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
	stalemate_dialog = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Stalemate"
	);
	g_signal_connect(stalemate_dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}
