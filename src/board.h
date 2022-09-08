#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "state.h"

extern board_state global_state;
extern RsvgHandle *image;

gboolean draw_callback (GtkWidget*, cairo_t*, gpointer);

#endif
