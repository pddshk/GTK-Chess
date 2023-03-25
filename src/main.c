#include "globals.h"
#include "gtkchessapp.h"

int main(int argc, char** argv)
{
	GtkApplication* app = gtk_application_new("org.gtk.gtkchess", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(gtkchess_app_activate), NULL);
	g_signal_connect(app, "startup", G_CALLBACK(gtkchess_app_startup), NULL);
	g_signal_connect(app, "shutdown", G_CALLBACK(gtkchess_app_shutdown), NULL);
	g_signal_connect(app, "open", G_CALLBACK(gtkchess_app_open), NULL);
	return g_application_run(G_APPLICATION(app), argc, argv);
}
