#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "state.h"

extern board_state game_state;

void load_textures(/*const char* pack*/);

gboolean draw_board(GtkWidget*, cairo_t*, gpointer);

#endif
