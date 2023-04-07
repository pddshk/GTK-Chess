#ifndef TYPEDECL_H
#define TYPEDECL_H
#include <gtk/gtk.h>

typedef struct {
    int castlings[4]; // Queenside | Kingside | queenside | kingside
    int side_to_move;
    int fifty_moves_counter;
    int move_counter;
    char field[8][9];
    // set when pawn is moved 2 steps forward
    // unset when not
    int enpassant_row, enpassant_col;
    int is_active;
    int flipped;
} game_state;

typedef struct
{
  game_state* field;      // поле данных
  struct tnode* parent;  // родитель
  GList* children;
  int hbox_status;
  int indent;
  GtkBox* hbox;
  GtkBox* vbox;
  char* last_move_notation;
} tnode;

typedef struct
{
  tnode* root;
  tnode* current;
} state_tree;

#endif
