#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include "state.h"
#include "board.h"

extern game_state state;
// extern int from_engine_manager;

void init_elements();

void flip_board(GtkButton*, gpointer);

void new_game(GtkButton*, gpointer);

#endif
