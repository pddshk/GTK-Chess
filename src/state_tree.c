#include "state_tree.h"

tnode* addnode(game_state _field, tnode *_parent, const char *last_move) 
{
    tnode* new_node =  (tnode*)malloc(sizeof(tnode)); 
    new_node->field = _field;
    new_node->children =  NULL;
    strcpy(new_node->last_move_notation, last_move);
    new_node->hbox = NULL;
    new_node->vbox = NULL;
    (*new_node).hbox_status=0;
    (*new_node).indent=0;
    if(_parent!= NULL)
    {
        new_node->parent = (struct tnode*)_parent;
        GList* elem = _parent->children;
        for( ; elem!=NULL; elem = elem->next) 
        {
            tnode* item = elem->data;
            if(tnode_equals(item,new_node)==0)//compare game state
            {
                return item;
            }
        }
        _parent->children =g_list_append(_parent->children, new_node);
    }
    
    return new_node;
}


void init_tree(game_state state)
{
    //tree =  (state_tree*)malloc(sizeof(state_tree)); 
    tnode* start_state_node = (tnode*)malloc(sizeof(tnode));
    start_state_node->field = state;
    start_state_node->parent = NULL;
    start_state_node->children = NULL;
    strcpy(start_state_node->last_move_notation, "begin");
    (*start_state_node).hbox_status=0;
    (*start_state_node).indent=0;
    start_state_node->hbox = NULL;
    start_state_node->vbox = NULL;
    tree.root = start_state_node;
    tree.current = start_state_node;
}

void destroy_tree()
{
    destroy_tnodes(tree.root);
    //free(tree);
}

void destroy_tnodes(tnode* node)
{
    if(g_list_length(node->children)!=0)
    {
        GList* element = node->children;
        for(; element!=NULL; element = element->next) 
        {
            tnode* item = element->data;
            destroy_tnodes(item);
        }
        g_list_free(element);
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
    if (node->field.side_to_move)
        sprintf(label, "%d... %s\n", --actual_move, node->last_move_notation);
    else 
        sprintf(label, "%d. %s  \n", actual_move, node->last_move_notation);
}
