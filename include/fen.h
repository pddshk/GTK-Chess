#ifndef FEN_H
#define FEN_H

#include "typedecl.h"
#include "gtk/gtk.h"

extern state_tree tree;

void paste_FEN(GtkButton*, GtkWidget*);

static game_state* FEN_to_game_state(const gchar*);
inline void raise_error(void);

#endif
