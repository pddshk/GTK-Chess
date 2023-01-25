#include "state_tree.h"
tnode* addnode(game_state* _field, tnode *_parent, char *last_move) 
{
  tnode* aboba =  (tnode*)malloc(48); 
  aboba->field = _field;
  aboba->children =  NULL;
  aboba->last_move_notation = last_move;
  if(_parent!= NULL)
  {
    aboba->parent = _parent;
    _parent->children =g_list_append(_parent->children, aboba);
  }
  return aboba;
}


state_tree* init_tree(game_state* state)
{
  /*if(aboba!= NULL)
  {
    destroy_tree(aboba);
  }*/
    state_tree* tree =  (state_tree*)malloc(48); 
    tnode* abobik = (tnode*)malloc(48);
    abobik->field = state;
    abobik->parent=NULL;
    abobik->children = NULL;
    abobik->last_move_notation = "beg";
    tree->root = abobik;
    tree->current = abobik;
    return tree;
   
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
    free(tnode->field);
    free(tnode->graphics);
    free(tnode->last_move_notation);
    g_list_free(tnode->children);
    free(tnode);
}

