#ifndef BOARD_H
#define BOARD_H

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "state.h"
#include "rules.h"

extern state_tree tree;
extern int flipped;
extern int pawn_promotion_row, pawn_promotion_col;
extern char pawn_promotion;

void init_textures();
void load_textures(const char* pack);

void
drag_begin (
    GtkWidget* self,
    GdkDragContext* context,
    gpointer user_data
);

gboolean
drag_motion (
    GtkWidget* self,
    GdkDragContext* context,
    gint x,
    gint y,
    guint time,
    gpointer user_data
);

gboolean
drag_failed (
    GtkWidget* self,
    GdkDragContext* context,
    GtkDragResult result,
    gpointer user_data
);

gboolean
drag_drop (
    GtkWidget* self,
    GdkDragContext* context,
    gint x, gint y,
    guint time,
    gpointer data
);

gboolean
board_clicked (
    GtkWidget* self,
    GdkEventButton *event,
    gpointer user_data
);

gboolean draw_board(GtkWidget*, cairo_t*, gpointer);

#endif
