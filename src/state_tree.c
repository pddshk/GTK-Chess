#include "state_tree.h"

tnode* addnode(game_state* _field, tnode *_parent, char *last_move) 
{
  tnode* aboba =  (tnode*)malloc(48); 
  aboba->field = _field;

  aboba->children =  NULL;
  aboba->last_move_notation = last_move;
  aboba->hbox = NULL;
 aboba->vbox = NULL;
 (*aboba).hbox_status=0;
 (*aboba).indent=0;
  if(_parent!= NULL)
  {
    aboba->parent = (struct tnode*)_parent;
    GList* elem = _parent->children;
    for( ; elem!=NULL; elem = elem->next) 
		{
      tnode* item = elem->data;
      char* label_1 = get_label(item);
      char* label_2 = get_label(aboba);
      if(strcmp(label_1,label_2)==0)//compare game state
      {
        free(label_1);
        free(label_2);
       

        return item;
      }
      free(label_1);
      free(label_2);
    }
    
    _parent->children =g_list_append(_parent->children, aboba);
  }
  return aboba;
}


state_tree* init_tree(game_state* state)
{
    state_tree* tree =  (state_tree*)malloc(48); 
    tnode* abobik = (tnode*)malloc(48);
    abobik->field = state;
    abobik->parent=NULL;
    abobik->children = NULL;
    abobik->last_move_notation = "beg";
    (*abobik).hbox_status=0;
    (*abobik).indent=0;
    abobik->hbox = NULL;
    abobik->vbox = NULL;
    tree->root = abobik;
    tree->current = abobik;
    return tree;
   
}
void destroy_tree(state_tree* aboba)
{
  if(aboba==NULL)
  return;
  destroy_tnodes(aboba->root);
  //free(aboba->root->last_move_notation);
  free(aboba);
  //puts("tree destroyed\n");
}
void destroy_tnodes(tnode* node)
{
  if(g_list_length(node->children)!=0)
	{
    GList* elem = node->children;
		for(; elem!=NULL; elem = elem->next) 
		{
      tnode* item = elem->data;
      destroy_tnodes(item);
    }
    g_list_free(elem);
     
  }
  
  //free(node->field);
  //free(node->hbox);
  //free(node->vbox);
  //free(node->last_move_notation);
  
  free(node);
  //free(node->last_move_notation);

}

char* get_label( tnode* node)
{
	char* label = malloc(sizeof(char)* 10);
	int actual_move = node->field->move_counter;
	if (node->field->side_to_move != 0) actual_move--;
	if (node->field->side_to_move) 
	{
		sprintf(label, "%d... %s\n", actual_move, node->last_move_notation);
	}
	else 
	{
		sprintf(label, "%d. %s  \n", actual_move, node->last_move_notation);
	}
	return label;

}