#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <goocanvas.h>

void init_board(GooCanvas*, GooCanvasItemModel*);

//void draw_board(GooCanvas*, GooCanvasItem*);

void resize_board(GtkWidget*, gpointer);

#endif
