#include "gtkchessapp.h"
#include "board.h"

enum _EngineState{
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
} engine_state;

void gtkchess_app_startup(GApplication *app, gpointer data)
{
    engine_manager = NULL;
	
	if (!start_engine_manager(engine_manager)){
		fputs("Error while starting engine!\n", stderr);
		exit(EXIT_FAILURE);
	}
    engine_state = ENGINE_IDLE;
	init_state(&state);
	init_textures();
	load_textures("classic");
}

void gtkchess_app_activate(GApplication *app, gpointer data)
{
    GList *windows=gtk_application_get_windows(GTK_APPLICATION(app));
    if (!windows){
        builder = builder_init();
        mainwindow=GTK_WINDOW(gtk_builder_get_object(builder, "MainWindow"));
        gtk_application_add_window(GTK_APPLICATION(app), mainwindow);
        gtk_widget_show_all(GTK_WIDGET(mainwindow));
    } else {
        gtk_window_present(mainwindow);
    }
}

void gtkchess_app_shutdown(GApplication *self, gpointer data)
{
    tell_engine_manager(QUIT, NULL, 0);
    if (G_IS_SUBPROCESS(engine_manager) && !g_subprocess_get_if_exited(engine_manager))
        g_subprocess_force_exit(engine_manager);
}

void gtkchess_app_open(GApplication* app, gpointer data)
{

}

int start_engine_manager(GSubprocess *engine_manager)
{
	engine_manager = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		NULL,
		"./engine_manager",
		NULL
	);
	if (!engine_manager) {
		fprintf(stderr, "Cannot create subprocess for engine manager! Engine would be unavailable.\n");
		return FALSE;
	}
	to_engine_manager = g_subprocess_get_stdin_pipe(engine_manager);
	GInputStream *from_engine_manager = g_subprocess_get_stdout_pipe(engine_manager);
	if (!to_engine_manager || !from_engine_manager) {
		fprintf(stderr, "Cannot create pipes for engine manager. Engine would be unavailable. Try relaunching application\n");
		return FALSE;
	}
	from_engine_manager_source = NULL;
	if (g_pollable_input_stream_can_poll(G_POLLABLE_INPUT_STREAM(from_engine_manager))){
		from_engine_manager_source = g_pollable_input_stream_create_source(
			G_POLLABLE_INPUT_STREAM(from_engine_manager),
			NULL
		);
		g_source_attach(from_engine_manager_source, NULL); // to default context
	} else {
		fprintf(stderr, "Cannot create pollable stream from engine manager!\n");
		return FALSE;
	}
	tell_engine_manager(LOAD_ENGINE, "stockfish", strlen("stockfish"));
	int code=0;
	size_t size=0;
	g_input_stream_read(from_engine_manager, &code, sizeof code, NULL, NULL);
	g_input_stream_read(from_engine_manager, &size, sizeof size, NULL, NULL);
	return code == DONE;
}

GtkBuilder *builder_init()
{
    GtkBuilder* builder=gtk_builder_new_from_resource("/org/gtk/gtkchess/window.glade");
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

	g_source_set_callback(
		from_engine_manager_source,
		G_SOURCE_FUNC(parse_engine_response),
		builder,
		NULL
	);
    return builder;
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
	state.flipped = flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
}

gboolean parse_engine_response(GObject* stream, gpointer data)
{
	int code=0;
	size_t nbytes=0;
	gssize nread;
	char buff[2048] = "";
	GError *error = NULL;
	g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		&code,
		sizeof code,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		&nbytes,
		sizeof nbytes,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	nread = g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		buff,
		nbytes,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	if (nread == G_IO_ERROR_WOULD_BLOCK) return TRUE;
	//handle message by code
	switch (code)
	{
	case INFO:
		printf("info %s\n", buff);
		break;
	case BESTMOVE:
		printf("bestmove %s\n", buff);
		break;
	default:
		break;
	}
	return TRUE;
}

void toggle_engine(GtkButton* self, gpointer data)
{
	switch (engine_state) {
		case ENGINE_IDLE:
			//puts("Starting engine!");
			tell_engine_manager(GO, NULL, 0);
			//g_output_stream_write(to_engine_manager, "go\n", (sizeof "go\n") - 1, NULL, NULL);
			engine_state = ENGINE_WORKING;
			gtk_button_set_label(self, "Stop");
			break;
		case ENGINE_WORKING:
			//puts("Stopping engine!");
			tell_engine_manager(STOP, NULL, 0);
			//g_output_stream_write(to_engine_manager, "stop\n", (sizeof "stop\n") - 1, NULL, NULL);
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

void tell_engine_manager(int type, const void* data, size_t size)
{
    g_output_stream_write(to_engine_manager, &type, sizeof type, NULL, NULL);
	g_output_stream_write(to_engine_manager, &size, sizeof size, NULL, NULL);
    if (size)
        g_output_stream_write(to_engine_manager, data, size, NULL, NULL);
    g_output_stream_flush(to_engine_manager, NULL, NULL);
}