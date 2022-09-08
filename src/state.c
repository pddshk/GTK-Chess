#include "state.h"
#include <string.h>

void init_state()
{
    strcpy(game_state.field[0], "rnbqkbnr");
    strcpy(game_state.field[1], "pppppppp");
    for (int i = 2; i < 6; strcpy(game_state.field[i++], "--------"));
    strcpy(game_state.field[6], "PPPPPPPP");
    strcpy(game_state.field[7], "RNBQKBNR");
    game_state.side_to_move = 0;
    game_state.fifty_moves_counter = 0;
    game_state.move_counter = 0;
    game_state.enpassant[0] = 0;
}

char get_field(const char* field)
{
    int i, j;
    switch (field[0]) {
        case 'a': i = 0; break;
        case 'b': i = 1; break;
        case 'c': i = 2; break;
        case 'd': i = 3; break;
        case 'e': i = 4; break;
        case 'f': i = 5; break;
        case 'g': i = 6; break;
        case 'h': i = 7; break;
    }
    j = field[1] - '0';
    return game_state.field[i][j];
}
