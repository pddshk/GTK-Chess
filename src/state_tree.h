#include <stdlib.h>
#include "state.h"
#include "arraylist.h"

typedef struct 
{
  tnode* root;
  tnode* current;
} tnode_tree;

typedef struct 
{
  game_state* field;           // поле данных
  tnode *parent;  // родитель
  arraylist *children; // правый потомок
}tnode;

tnode * addnode(game_state* _field, tnode *_parent);
void freemem(tnode *tree);

void treeprint(tnode *tree);

tnode_tree* create_tree();