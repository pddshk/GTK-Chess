#include "gui.h"

void init_elements()
{
	init_state(&state);
	load_textures();
	GtkBuilder* builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GtkWidget *Board = GTK_WIDGET(gtk_builder_get_object(builder, "Board"));
	GdkPixbuf *empty_icon = gdk_pixbuf_new (GDK_COLORSPACE_RGB, 0, 8, 1, 1);
	GtkTargetEntry *board_entry = gtk_target_entry_new(
		"GtkDrawingArea",
		GTK_TARGET_SAME_WIDGET,
		0
	);
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
	g_signal_connect(Board, "drag-begin", G_CALLBACK(drag_begin), empty_icon);
	g_signal_connect(Board, "drag-motion", G_CALLBACK(drag_motion), NULL);
	g_signal_connect(Board, "drag-failed", G_CALLBACK(drag_failed), NULL);

	GtkWidget **dialogs = g_new(GtkWidget*, 4);
	// mate_dialog
	dialogs[0] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Mate"
	);
	g_signal_connect(dialogs[0], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// stalemate_dialog
	dialogs[1] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Stalemate"
	);
	g_signal_connect(dialogs[1], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// insufficient_material_dialog
	dialogs[2] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Insufficient material"
	);
    g_signal_connect(dialogs[2], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// threefold_repetition_dialog
	dialogs[3] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Threefold repetition"
	);
	g_signal_connect(dialogs[3], "response", G_CALLBACK(gtk_widget_hide), NULL);

	g_signal_connect(Board, "drag-drop", G_CALLBACK(drag_drop), dialogs);

	GObject *FlipBoardButton=gtk_builder_get_object(builder, "FlipBoard");
	g_signal_connect(FlipBoardButton, "clicked", G_CALLBACK(flip_board), Board);

	GObject *NewGameButton=gtk_builder_get_object(builder, "NewGame");
	g_signal_connect(NewGameButton, "clicked", G_CALLBACK(new_game), Board);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}
