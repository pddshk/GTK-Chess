#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <goocanvas.h>

void init_board(GooCanvas*);

void resize_board(GtkWidget*, gpointer);

#endif
