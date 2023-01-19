#include <stdlib.h>
#include "state.h"
#include <glib.h>
#include <stdio.h>

typedef struct 
{
  tnode* root;
  tnode* current;
} tnode_tree;

typedef struct 
{
  game_state* field;      // поле данных
  tnode *parent;  // родитель
  GList* children;
}tnode;



tnode * addnode(game_state* _field, tnode *_parent);
tnode_tree* create_tree();
void destroy_tree(tnode* tnode);