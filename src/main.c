#include <gtk/gtk.h>

int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	GtkBuilder* builder = gtk_builder_new_from_file("src/window.glade");
	GObject* window = gtk_builder_get_object(builder, "MainWindow");
    gtk_widget_show(GTK_WIDGET(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();
	return 0;
}
