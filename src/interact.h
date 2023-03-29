#ifndef INTERACT_H
#define INTERACT_H

enum {
    // both
    NOP,
    //to engine manager
    REQUEST_STATE,
    LOAD_ENGINE,
    UNLOAD_ENGINE,
    SET_MODE,
    NEW_GAME,
    SET_POSITION,
    UPDATE_POSITION,
    GO,
    STOP,
    MOVE_NOW,
    QUIT,
    // from engine manager
    DONE,
    LINE,
    INFO,
    BESTMOVE,
    BESTMOVE_PONDER
};

#endif
