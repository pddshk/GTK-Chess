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

typedef struct 
{
    char event[256];
    char site[256];
    char date[256];
    char round[256];
    char white[256];
    char black[256];
    char result[256];
    char annotator[256];
    char plyCount[256];
    char timeControl[256];
    char time[256];
    char termination[256];
    char mode[256];
    char setUp[256];
    char FEN[256];
    state_tree movetree;
} game_info;


enum _EngineState{
	ENGINE_NONE,
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
};

#endif
