#include "game_info.h"
#include "state_tree.h"

void init_casual_game(game_info* game) {
    init_tree(&game->movetree, NULL);
}
