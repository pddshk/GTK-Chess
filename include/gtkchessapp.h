#ifndef GTKCHESSAPP_H
#define GTKCHESSAPP_H

#include <gtk/gtk.h>
#include "typedecl.h"
#include "interact.h"

GtkBuilder *builder_init(void);

void new_game(GtkButton*, gpointer);

void gtkchess_app_startup(GApplication*, gpointer);
void gtkchess_app_shutdown(GApplication*, gpointer);
void gtkchess_app_activate(GApplication*, gpointer);
void gtkchess_app_open(GApplication*, gpointer);

#endif
