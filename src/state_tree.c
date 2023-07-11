#include "state_tree.h"
#include "state.h"

// allocates
tnode* addnode(game_state _field, tnode *_parent, const char *last_move) 
{
    tnode* new_node = malloc(sizeof(tnode)); 
    new_node->state = _field;
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
            if (tnode_equals(item, new_node)) {
                return item;
            }
        }
        _parent->children = g_list_append(_parent->children, new_node);
    }
    return new_node;
}

// init tree from given state if state is NULL, then init fro starting postion
// allocates
void init_tree(state_tree* tree, const game_state* state)
{
    tree->root = tree->current = malloc(sizeof(tnode));
    tree->root->parent = NULL;
    tree->root->children = NULL;
    strcpy(tree->root->last_move_notation, "begin");
    tree->root->hbox_status = 0;
    tree->root->indent = 0;
    tree->root->hbox = NULL;
    tree->root->vbox = NULL;
    if (state)
        tree->root->state = *state;
    else
        init_state(&tree->root->state);
}

void clear_tree(state_tree* tree)
{
    if (tree->root)
        destroy_tnode(tree->root);
    tree->current = NULL;
}

void destroy_tnode(tnode* node)
{
    if(g_list_length(node->children) != 0)
    {
        GList* element = node->children;
        while(element) {
            destroy_tnode(element->data);
            element = element->next;
        }
        g_list_free(element);
    }
    free(node);
}

int tnode_equals(const tnode* tnode_a, const tnode* tnode_b)//compares tnodes: returns 0 if equal -1 - if not
{
    return states_equals(&tnode_a->state, &tnode_b->state);
}

void get_label(tnode* node, char* label)
{
    int actual_move = node->state.move_counter;
    if (node->state.side_to_move)
        sprintf(label, "%d... %s\n", --actual_move, node->last_move_notation);
    else 
        sprintf(label, "%d. %s  \n", actual_move, node->last_move_notation);
}
