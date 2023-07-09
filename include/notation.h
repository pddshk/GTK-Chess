#ifndef NOTATION_H
#define NOTATION_H

#include <gtk/gtk.h>
#include "typedecl.h"

void select_state(GtkButton*, tnode*);
void show_notation(const state_tree*);

void show_state(tnode*, int);
gchar* get_sign(int number, char symbol);

#endif
