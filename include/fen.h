#ifndef FEN_H
#define FEN_H

#include "typedecl.h"
#include "gtk/gtk.h"

void paste_FEN(GtkButton*, GtkWidget*);

static game_state* FEN_to_game_state(const gchar*);
inline void raise_error(void);

void copy_FEN(GtkButton*, gpointer);
static void game_state_to_FEN(const game_state* state, char* fen);

#endif
