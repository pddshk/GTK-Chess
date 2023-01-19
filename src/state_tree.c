#include "state_tree.h"

tnode * addnode(game_state* _field, tnode *_parent) 
{
  tnode* aboba =  (tnode*)malloc(sizeof(aboba)); 
  aboba->field = _field;
  aboba->children =  NULL;
  
  if(_parent!= NULL)
  {
    aboba->parent = _parent;
    _parent->children =g_list_append(_parent->children, aboba);
  }

  return aboba;
}


tnode_tree* create_tree()
{
    tnode_tree* aboba =  (tnode_tree*)malloc(sizeof(aboba)); 
    tnode* abobik = (tnode*)malloc(sizeof(aboba));
    init_state(abobik->field);
    abobik->parent=NULL;
    abobik->children = NULL;
    aboba->root = abobik;
    aboba->current = abobik;
    return aboba;
}

void destroy_tree(tnode* tnode)
{
    for(int i=0; i<(g_list_length (tnode->children));i++)
    {
        destroy_tree(g_list_nth(tnode->children,i));
    }
    free(tnode->field);
    g_list_free(tnode->children);
    free(tnode);
}

