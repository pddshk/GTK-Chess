#ifndef GLOBALS_H
#define GLOBALS_H

struct _settings{
    char engine[32];
    char textures[32];
} settings;
game_state state;
GOutputStream *to_engine_manager;
//my var
state_tree tree;
// GInputStream *from_engine_manager;
GSource *from_engine_manager_source;
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

#endif
