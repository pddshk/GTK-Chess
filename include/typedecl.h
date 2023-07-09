#ifndef TYPEDECL_H
#define TYPEDECL_H
#include <gtk/gtk.h>

#define WHITE (1)
#define BLACK (0)

typedef struct {
    // TODO fix to be KQkq
    int castlings[4]; // Queenside | Kingside | queenside | kingside
    int side_to_move;
    int fifty_moves_counter;
    int move_counter;
    // set when pawn is moved 2 steps forward
    // unset when not
    int enpassant_row, enpassant_col;
    int is_active; //?????
    char field[8][9];
} game_state;

struct tnode; 

typedef struct
{
    game_state state;
    struct tnode* parent;
    GList* children;
    int hbox_status;// remove
    int indent;     // remove
    GtkBox* hbox;   // remove
    GtkBox* vbox;   // remove
    char last_move_notation[10];
} tnode;

typedef struct
{
    tnode* root;
    tnode* current;
} state_tree;

enum _EngineState{
	ENGINE_NONE,
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
};

#endif
