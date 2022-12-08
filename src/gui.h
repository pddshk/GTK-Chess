#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "state.h"
#include "board.h"

extern game_state state;
extern GOutputStream *to_engine_manager;
extern GSource *from_engine_manager_source;
// extern GInputStream *from_engine;

void init_elements(char*);

void flip_board(GtkButton*, gpointer);

void new_game(GtkButton*, gpointer);

void toggle_engine(GtkButton*, gpointer);

gboolean parse_engine_response(GObject*, gpointer);

void tell_engine_manager(int, const void*, size_t);

#endif
