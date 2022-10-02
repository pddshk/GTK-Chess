#ifndef GLOBALS_H
#define GLOBALS_H

game_state state;
GOutputStream *to_engine;
GInputStream *from_engine;
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

#endif
