#ifndef ENGINE_H
#define ENGINE_H
#include <gio/gio.h>

#define UCI "uci\n"
#define ISREADY "isready\n"

char engine_name[128] = "stockfish";
/*
Engine manager process is responsible for creating separate process for
UCI engine and properly set it up for work. It also handles output from
engine and passes it to GUI
*/

enum EngineState {
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
};

static enum EngineState engine_state = ENGINE_IDLE;

typedef struct {
    char exec_path[128];
    int nparams;
    char (*param_names)[32];
    char (*param_values)[32];
} engine_params;

struct engine_response{
    int cp;
    int depth;
    char *currmove;
    char *line;
};

GOutputStream *to_engine;
GInputStream *from_engine;

static void tell_engine(const char*);
static int init_engine(engine_params*);
// static void skip_output();
// void start_stop(GtkBox*);
void start_stop();
void stop_engine();
void main_loop();
// gboolean parse_engine_response(gint, GIOCondition, gpointer);

#endif
