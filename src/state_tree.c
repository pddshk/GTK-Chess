#include "state_tree.h"



tnode * addnode(game_state* _field, tnode *_parent) 
{
  tnode* aboba =  (tnode*)malloc(sizeof(aboba)); 
  aboba->field = _field;
  aboba->children = arraylist_create();
  if(_parent!= NULL)
  {
    aboba->parent = _parent;
    arraylist_add(_parent->children, aboba);
  }

  return aboba;
}

tnode_tree* create_tree()
{
    tnode_tree* aboba =  (tnode_tree*)malloc(sizeof(aboba)); 
    tnode* abobik = (tnode*)malloc(sizeof(aboba));
    init_state(abobik->field);
    abobik->parent=NULL;
    abobik->children = arraylist_create();
    aboba->root = abobik;
    aboba->current = abobik;
}

void destroy_tree(tnode* tnode)
{
    for(int i=0; i<(tnode->children->size);i++)
    {
        destroy_tree(arraylist_get(tnode->children,i));
    }
    free(tnode->field);
    free(tnode->children);
    free(tnode);
}
