#ifndef ENGINE_H
#define ENGINE_H
#include <gtk/gtk.h>
#include "state.h"

extern game_state state;
extern int from_engine, to_engine;
static int engine_status = 0;

struct engine_response{
    int cp;
    int depth;
    char *currmove;
    char *line;
};

void tell_engine(const char*);
void init_engine();
void skip_output();
void start_stop(GtkBox*);
void stop_engine();
gboolean update_variants(GtkBox*);

#endif
