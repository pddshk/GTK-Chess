#include "gtkchessapp.h"
#include "board.h"
#include "state_tree.h"
#include "engine_controls.h"
#include "notation.h"

extern int flipped;
extern GOutputStream *to_engine_manager;
extern GSource *from_engine_manager_source;
extern GSubprocess *engine_manager;
extern GtkBuilder *builder;
extern GtkWindow *mainwindow;
extern int nvariations;
extern GtkLabel *variations[4];
extern GtkWidget *dialogs[4];
extern state_tree tree;
extern GtkBox* vbox;
extern enum _EngineState engine_state;

void gtkchess_app_startup(
	__attribute_maybe_unused__ GApplication *app,
	__attribute_maybe_unused__ gpointer data)
{
    engine_manager = NULL;
	
	if (!start_engine_manager(engine_manager)){
		fputs("Error while starting engine!\n", stderr);
		engine_state = ENGINE_NONE;
	} else {
		engine_state = ENGINE_IDLE;
	}
	init_tree(&tree, NULL);
}

void gtkchess_app_activate(
	GApplication *app,
	__attribute_maybe_unused__ gpointer data)
{
    GList *windows=gtk_application_get_windows(GTK_APPLICATION(app));
    if (!windows){
        builder = builder_init();
        mainwindow = GTK_WINDOW(gtk_builder_get_object(builder, "MainWindow"));
        gtk_application_add_window(GTK_APPLICATION(app), mainwindow);
        gtk_widget_show_all(GTK_WIDGET(mainwindow));
    } else {
        gtk_window_present(mainwindow);
    }
}

void gtkchess_app_shutdown(
	__attribute_maybe_unused__ GApplication *self,
	__attribute_maybe_unused__ gpointer data)
{
	g_source_remove(g_source_get_id(from_engine_manager_source));
    tell_engine_manager(QUIT, NULL, 0);
    if (G_IS_SUBPROCESS(engine_manager) &&
			!g_subprocess_get_if_exited(engine_manager)) {
		fprintf(stderr, "Warning: Force exiting engine manager");
        g_subprocess_force_exit(engine_manager);
	}
	clear_tree(&tree);
}

void gtkchess_app_open(
	__attribute_maybe_unused__ GApplication* app,
	__attribute_maybe_unused__ gpointer data)
{
}

GtkBuilder *builder_init(void)
{
	GtkCssProvider* provider = gtk_css_provider_new();
	GdkDisplay* display = gdk_display_get_default();
	GdkScreen* screen = gdk_display_get_default_screen (display);
	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_resource(provider, "/org/gtk/gtkchess/selected.css");

	//cppcheck-suppress shadowVariable
    GtkBuilder *builder=gtk_builder_new_from_resource("/org/gtk/gtkchess/window.glade");
    GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
	vbox = GTK_BOX(gtk_builder_get_object(builder, "Notation"));
    GtkWidget *Board = GTK_WIDGET(gtk_builder_get_object(builder, "Board"));

    GdkPixbuf *empty_icon = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, 1, 1);
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

	gtk_builder_connect_signals(builder, NULL);
	//GObject *EngineToggler=gtk_builder_get_object(builder, "EngineToggler");

	g_source_set_callback(
		from_engine_manager_source,
		G_SOURCE_FUNC(parse_engine_response),
		builder,
		NULL
	);
	variations[0] = GTK_LABEL(gtk_builder_get_object(builder, "Variation0"));
	variations[1] = GTK_LABEL(gtk_builder_get_object(builder, "Variation1"));
	variations[2] = GTK_LABEL(gtk_builder_get_object(builder, "Variation2"));
	variations[3] = GTK_LABEL(gtk_builder_get_object(builder, "Variation3"));
	init_textures();
	// load_textures("classic");
    return builder;
}

void new_game(__attribute_maybe_unused__ GtkButton* button, gpointer Board)
{
	clear_tree(&tree);
	init_tree(&tree, NULL);
	gtk_widget_queue_draw(GTK_WIDGET(Board));
	show_notation(&tree);
}
