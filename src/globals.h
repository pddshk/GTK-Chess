#ifndef GLOBALS_H
#define GLOBALS_H
#include "typedecl.h"
#include <gtk/gtk.h>

game_state state;
GOutputStream *to_engine_manager;
// GInputStream *from_engine_manager;
GSubprocess *engine_manager;
GSource *from_engine_manager_source;
GtkBuilder *builder;
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';
#endif
