#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "state.h"
#include "board.h"

extern game_state state;
extern GOutputStream *to_engine;
// extern GInputStream *from_engine;

void init_elements();

void flip_board(GtkButton*, gpointer);

void new_game(GtkButton*, gpointer);

void toggle_engine(GtkButton*, gpointer);

#endif
