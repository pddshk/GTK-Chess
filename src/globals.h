#ifndef GLOBALS_H
#define GLOBALS_H

game_state state;
int from_engine[2], to_engine[2];
char *engine="bin/stockfish_15";
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

#endif
