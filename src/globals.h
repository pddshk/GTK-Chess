#ifndef GLOBALS_H
#define GLOBALS_H

game_state state;
GOutputStream *to_engine_manager;
// GInputStream *from_engine_manager;
GSource *from_engine_manager_source;
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

#endif
