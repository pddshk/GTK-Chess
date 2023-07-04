#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H

#include <gtk/gtk.h>

gboolean parse_engine_response(GObject*, gpointer);
void tell_engine_manager(int, const void*, size_t);
void toggle_engine(GtkButton*, gpointer);
int start_engine_manager(GSubprocess*);
void add_variation(GtkButton*, gpointer);
void rm_variation(GtkButton*, gpointer);

#endif
