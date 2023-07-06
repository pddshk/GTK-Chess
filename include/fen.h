#ifndef FEN_H
#define FEN_H

#include "typedecl.h"
#include "gtk/gtk.h"

void paste_FEN(GtkButton*, GtkWidget*);
game_state* FEN_to_game_state(const gchar*);

static int read_field(game_state* state, const char** fen);
static int read_side_to_move(game_state* state, const char** fen);
static int read_castlings(game_state *state, const char** fen);
static int read_enpassant(game_state* state, const char** fen);
static int read_move_counters(game_state* state, const char **fen);

void copy_FEN(GtkButton*, gpointer);
void game_state_to_FEN(const game_state* state, char* fen);

static void write_board(const game_state* state, char** fen);
static void write_side_to_move(const game_state* state, char** fen);
static void write_castlings(const game_state* state, char** fen);
static void write_enpassant(const game_state* state, char** fen);
static void write_counters(const game_state* state, char** fen);

#endif
