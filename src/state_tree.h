#ifndef STATE_TREE_H
#define STATE_TREE_H
#include "typedecl.h"
extern state_tree tree;

void init_tree(game_state);
void destroy_tree();
tnode * addnode(game_state, tnode*, const char* last_move);
void destroy_tnodes(tnode*);
int tnode_equals(tnode* , tnode* );
void get_label(tnode* ,char*);            //returns move text for the view

#endif
