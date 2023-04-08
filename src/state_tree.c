#include "state_tree.h"

tnode* addnode(game_state _field, tnode *_parent, const char *last_move) 
{
    tnode* aboba =  (tnode*)malloc(sizeof(tnode)); 
    aboba->field = _field;
    aboba->children =  NULL;
    strcpy(aboba->last_move_notation, last_move);
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
            if(tnode_equals(item,aboba)==0)//compare game state
            {
                return item;
            }
        }
        _parent->children =g_list_append(_parent->children, aboba);
    }
    
    return aboba;
}


state_tree* init_tree(game_state state)
{
    state_tree* tree =  (state_tree*)malloc(sizeof(state_tree)); 
    tnode* abobik = (tnode*)malloc(sizeof(tnode));
    abobik->field = state;
    abobik->parent = NULL;
    abobik->children = NULL;
    strcpy(abobik->last_move_notation, "begin");
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
    if(aboba==NULL) return;
    destroy_tnodes(aboba->root);
    free(aboba);
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
    
    free(node);
}

int tnode_equals(tnode* tnode_a, tnode* tnode_b)//compares tnodes: returns 0 if equal -1 - if not
{
    if(tnode_a->field.side_to_move==tnode_b->field.side_to_move &&
    tnode_a->field.move_counter==tnode_b->field.move_counter &&
    strcmp(tnode_a->last_move_notation,tnode_b->last_move_notation)==0)
        return 0;
    else
        return -1;
}

void get_label(tnode* node, char* label)
{
    int actual_move = node->field.move_counter;
    if (node->field.side_to_move != 0) actual_move--;
    if (node->field.side_to_move)
        sprintf(label, "%d... %s\n", actual_move, node->last_move_notation);
    else 
        sprintf(label, "%d. %s  \n", actual_move, node->last_move_notation);
}