#ifndef STATE_TREE_H
#define STATE_TREE_H

#include "typedecl.h"

void init_tree(state_tree*, const game_state*);
void clear_tree(state_tree*);

tnode * addnode(game_state, tnode*, const char* last_move);
void destroy_tnode(tnode*);

int tnode_equals(const tnode*, const tnode*);

//returns move text for the view
void get_label(tnode*, char*);

#endif
