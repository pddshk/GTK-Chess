#include <stdlib.h>
#include "state.h"
#include <glib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#ifndef STATE_TREE_H
#define STATE_TREE_H

struct tnode;

typedef struct
{
  game_state* field;      // поле данных
  struct tnode* parent;  // родитель
  GList* children;
  GtkBox* graphics;
} tnode;

typedef struct
{
  tnode* root;
  tnode* current;
} state_tree;

extern state_tree tree;

void init_tree(state_tree*, game_state*);
void destroy_tree(state_tree*);
tnode * addnode(game_state*, tnode *);
void destroy_tnodes(tnode*);

#endif