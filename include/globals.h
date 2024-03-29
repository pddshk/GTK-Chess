#ifndef GLOBALS_H
#define GLOBALS_H
#include "typedecl.h"
#include <gtk/gtk.h>

int flipped;
GOutputStream *to_engine_manager;
//my var
GtkBuilder* builder;
GtkBox* vbox;
game_info game;
state_tree* const tree = &(game.movetree);
//state_tree tree;
//GObject* scrl_window;
//GObject* viewport;
// GInputStream *from_engine_manager;
GSubprocess *engine_manager;
GSource *from_engine_manager_source;
GtkBuilder *builder;
GtkWindow *mainwindow;
int nvariations = 1;
GtkLabel *variations[4];
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';
GtkWidget *dialogs[4];
enum _EngineState engine_state;
#endif
