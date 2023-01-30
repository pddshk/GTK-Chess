#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "state.h"
#include "state_tree.h"
#include "board.h"

extern game_state state;
//
extern GtkBuilder* builder;
extern state_tree *tree;
//
extern GOutputStream *to_engine_manager;
extern GSource *from_engine_manager_source;
// extern GInputStream *from_engine;

char* concat(char *s1, char *s2);

char* get_sign(int number);

char* get_label( tnode* node);

void init_elements(char*);

void flip_board(GtkButton*, gpointer);

void new_game(GtkButton*, gpointer);

void select_state(GtkButton*, gpointer);

void toggle_engine(GtkButton*, gpointer);
void show_state(tnode*,int);

static gboolean parse_engine_response(GObject*, gpointer);

#endif
