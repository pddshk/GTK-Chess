#ifndef NOTATION_H
#define NOTATION_H

#include <gtk/gtk.h>
#include "typedecl.h"

gchar* get_sign(int number,char symbol);
void select_state(GtkButton*, gpointer);
void show_state(tnode*, int);

#endif
