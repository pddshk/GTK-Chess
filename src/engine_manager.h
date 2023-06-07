#ifndef ENGINE_H
#define ENGINE_H
#include <gio/gio.h>

#define UCI "uci\n"
#define ISREADY "isready\n"

char engine_name[128] = ""; // = "stockfish";

/*
Engine manager process is responsible for creating separate process for
UCI engine and properly set it up for work. It also handles output from
engine and passes it to GUI
*/

enum EngineState {
    ENGINE_NONE,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
};

static enum EngineState engine_state = ENGINE_IDLE;

typedef struct _engine_params{
    char exec_path[128];
    int nparams;
    char (*param_names)[32];
    char (*param_values)[32];
} engine_params;

GOutputStream *to_engine;
GInputStream *from_engine;

static int init_engine(engine_params*);

static int run_engine(GSubprocess*, engine_params*);

void clear_params(engine_params*);

// static void skip_output();
// void start_stop(GtkBox*);
void start_stop(void);
void stop_engine(void);
static void main_loop(void);
// gboolean parse_engine_response(gint, GIOCondition, gpointer);

void tell_gui(int, const void*, size_t);
void tell_engine(const char*);

#endif
