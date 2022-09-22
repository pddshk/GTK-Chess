#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include "state.h"
#include "board.h"

extern game_state state;
extern GdkPixbuf *empty_icon;
extern GtkWidget *Board;

void init_elements();

#endif
