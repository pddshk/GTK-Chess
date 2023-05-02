#ifndef GTKCHESSAPP_H
#define GTKCHESSAPP_H

#include <gtk/gtk.h>
#include "typedecl.h"
#include "interact.h"

extern int flipped;
extern GOutputStream *to_engine_manager;
extern GSource *from_engine_manager_source;
extern GSubprocess *engine_manager;
extern GtkBuilder *builder;
extern GtkWindow *mainwindow;
extern int nvariations;
extern GtkLabel *variations[4];
extern GtkWidget *dialogs[4];
extern state_tree tree;
extern GtkBox* vbox;

GtkBuilder *builder_init();

void flip_board(GtkButton*, gpointer);

void new_game(GtkButton*, gpointer);

void toggle_engine(GtkButton*, gpointer);

gboolean parse_engine_response(GObject*, gpointer);

void tell_engine_manager(int, const void*, size_t);

int start_engine_manager(GSubprocess*);

void add_variation(GtkButton*, gpointer);
void rm_variation(GtkButton*, gpointer);

void gtkchess_app_startup(GApplication*, gpointer);
void gtkchess_app_shutdown(GApplication*, gpointer);
void gtkchess_app_activate(GApplication*, gpointer);
void gtkchess_app_open(GApplication*, gpointer);

gchar* get_sign(int number);
void paste_FEN(GtkButton*, gpointer);
void paste_PGN(GtkButton*, gpointer);
void select_state(GtkButton*, gpointer);
void show_state(tnode*,int);

#endif
