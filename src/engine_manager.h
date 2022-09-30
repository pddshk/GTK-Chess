#ifndef ENGINE_H
#define ENGINE_H
#include <gtk/gtk.h>
#include <glib-unix.h>
#include "state.h"

/*
Engine manager process is responsible for creating separate process for
UCI engine and properly set it up for work. It also handles output from
engine and passes it to GUI
*/

extern game_state state;

enum EngineState {
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
};
static enum EngineState engine_state = ENGINE_IDLE;

// enum MessageType {
//     ENGINE
// }

typedef struct {
    char exec_path[128];
    char exec_file[16];
    char (*param_names)[16];
    char (*param_values)[32];
} engine_params;

struct engine_response{
    int cp;
    int depth;
    char *currmove;
    char *line;
};

void engine_manager_main(int, int);

static void tell_engine(int, const char*);
static void init_engine();
static void skip_output();
void start_stop(GtkBox*);
void stop_engine();
gboolean parse_engine_response(gint, GIOCondition, gpointer);

#endif
