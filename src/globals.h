#ifndef GLOBALS_H
#define GLOBALS_H

game_state state;
GdkPixbuf *empty_icon;
GtkWidget *mate_dialog,
          *stalemate_dialog,
          *insufficient_material_dialog,
          *threefold_repetition_dialog;
int from_engine[2], to_engine[2];
char *engine="bin/stockfish_15";
GtkWidget *Board;
int pawn_promotion_row = -1, pawn_promotion_col = -1;
char pawn_promotion = '-';

#endif
