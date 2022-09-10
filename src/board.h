#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "state.h"

extern game_state state;
extern GtkTargetList* board_target;
extern GtkGestureDrag* drag_handler;

void load_textures(/* const char* pack */);

gboolean
drag_drop (
  GtkWidget* self,
  GdkDragContext* context,
  gint x, gint y,
  guint time,
  gpointer data
);

gboolean
drag_start(GtkWidget *widget,
    GdkEventMotion *event,
    gpointer data);

gboolean draw_board(GtkWidget*, cairo_t*, gpointer);

#endif
