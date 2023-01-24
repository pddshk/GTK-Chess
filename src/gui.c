#include "gui.h"
#include <glib-unix.h>
#include <string.h>
//#include "engine.h"

GtkBuilder* builder;

enum _EngineState{
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
} engine_state;

void init_elements(char* textures)
{
	engine_state = ENGINE_IDLE; 
	init_state(&state);
	//
	init_tree(&tree, &state);
	//
	init_textures();
	load_textures(textures);
	builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GtkWidget *Board = GTK_WIDGET(gtk_builder_get_object(builder, "Board"));//220
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
	g_signal_connect(Board, "drag-begin", G_CALLBACK(drag_begin), empty_icon);

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
	gtk_builder_connect_signals(builder, NULL);
	//GObject *EngineToggler=gtk_builder_get_object(builder, "EngineToggler");

	g_source_set_callback (
		from_engine_manager_source,
		G_SOURCE_FUNC(parse_engine_response),
		builder,
		NULL
	);

	gtk_widget_show(GTK_WIDGET(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void flip_board(GtkButton* button, gpointer Board)
{
	state.flipped = !state.flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
}

void new_game(GtkButton* button, gpointer Board)
{
	int flipped = state.flipped;
	init_state(&state);
	//
	init_tree(&tree, &state);
	//
	state.flipped = flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
}

gboolean parse_engine_response(GObject* stream, gpointer data)
{
	gssize nread;
	char buff[2048] = "";
	GError *error = NULL;
	nread = g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		buff,
		sizeof buff,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	if (nread == G_IO_ERROR_WOULD_BLOCK) return TRUE;
	puts(buff);
	return TRUE;
}

void toggle_engine(GtkButton* self, gpointer data)
{
	switch (engine_state) {
		case ENGINE_IDLE:
			//puts("Starting engine!");
			g_output_stream_write(to_engine_manager, "go\n", (sizeof "go\n") - 1, NULL, NULL);
			engine_state = ENGINE_WORKING;
			gtk_button_set_label(self, "Stop");
			break;
		case ENGINE_WORKING:
			//puts("Stopping engine!");
			g_output_stream_write(to_engine_manager, "stop\n", (sizeof "stop\n") - 1, NULL, NULL);
			engine_state = ENGINE_IDLE;
			gtk_button_set_label(self, "Go");
			break;
		case ENGINE_OFF:
			//puts("Engine is off!");
			break;
		case ENGINE_ERROR:
			puts("Engine is broken!");
			break;
	}
}



void show_state_tree(GtkWidget *textArea)
{
	GtkTextBuffer* tb = gtk_text_buffer_new (NULL);
	const gchar *text = "\ns\ns\ns\ns\ns\ns\ns\ns\ns\ns";
	gtk_text_buffer_set_text (tb,text,strlen(text));
	gtk_text_view_set_buffer(textArea, tb);
	return;
}

/*void show_state_tree(GtkVBox *notationBox) {
	GList* children = gtk_container_get_children(notationBox);
	g_list_foreach(list, (GFunc)destroy, NULL);
	GtkHBox
	gtk_box_pack_end(notationBox, );
	tnode* root = tree.root;
	for (int i = 0; i < g_list_length(root->children); i++) {

	}
}*/

void print_notation(const gchar *text) {
	GtkWidget *textArea = GTK_WIDGET(gtk_builder_get_object(builder, "Notation"));
	//GtkTextBuffer * tb = gtk_text_buffer_new (NULL);
	GtkTextBuffer * tb = gtk_text_view_get_buffer(textArea);
	GtkTextIter end_iter;
	gtk_text_buffer_get_end_iter(tb, &end_iter);
	gtk_text_buffer_insert(tb, &end_iter, text, strlen(text));
	gtk_text_view_set_buffer(textArea, tb);
}