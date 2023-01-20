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


void init_tree(state_tree* aboba, game_state* state)
{
  /*if(aboba!= NULL)
  {
    destroy_tree(aboba);
  }*/
    aboba =  (state_tree*)malloc(sizeof(aboba)); 
    tnode* abobik = (tnode*)malloc(sizeof(aboba));
    abobik->field = state;
    abobik->parent=NULL;
    abobik->children = NULL;
    aboba->root = abobik;
    aboba->current = abobik;
   
}
void destroy_tree(state_tree* aboba)
{
  //free(aboba);
  destroy_tnodes(aboba->root);
}
void destroy_tnodes(tnode* tnode)
{
    for(int i=0; i<(g_list_length (tnode->children));i++)
    {
        destroy_tnodes(g_list_nth(tnode->children,i)->data);
    }
    //free(tnode->field);
    g_list_free(tnode->children);
    free(tnode);
}

