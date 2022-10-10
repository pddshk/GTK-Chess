# Overview
UI is based on ChessBase for windows.

Main features (✗ - not implemented, ✓ - implemented):
    ✗ Application settings (read/write from config file)
    ✗ Play game against engine from any position
    ✗ Analyse position with engine
    ✗ Analyse position with engine using branching (deep analysis)
    ✗ Select and configure engine
    ✗ Work with chess notation
    ✗ Work with chess games databases
    ✗ Battle different engines (from any position)
    ✗ User can load different textures for pieces and board

Main parts:
    1. Engine manager
    2. Notation editor
    3. Chess Board

## Engine manager
### As separate process
Engine manager is 2-thread process that fork+exec engine child process and
synchronously communicates with it. It manages the whole workcycle of different
UCI engines, i.e. it **loads** engine, **initializes engine parameters**,
**transfers** commands and information from GUI to engine and vice versa,
correctly **shuts down** engine, and so on.

#### Features implemented:
    - run engine in child process
    - initialize engine parameters read from $enginename.conf
    - transfers commands from stdin to engine subprocess
    - performs all communication stuff synchronously
#### Features to be implemented:
    - store position state in process
    - API to get/send moves from/to GUI and engine
    - ability to turn off engine and wait for commands from gui (to load engine again)
    - ability to load another engine
    - engine battle mode

### As part of GUI
Engine manager should be controlled from GUI, so there should be some form/widget
that configures work of engine manager. I.e. sets default engine, adds and removes
engines, and configures engines parameters.
#### Features implemented
    - none
### Features to be implemented
    - CRUD engines list in some widget
    - configure engines (update config files)


## Notation editor
Notation editor is a part of project that is responsible for **showing** game
notation and **saving/loading** game files.

### Features implemented:
    - none
### Features to be implemented:
    - Store game notation in some way
        it seems to be good idea, to store it some way that could be
        corresponding with current `game_state` type. This would allow to
        perform threefold repetition check (Chess board part)
        The notation should satisfy following requirements:
            - Any form of notation could be chosen as a setting, i.e
              1. g1f3
              1. Ng1f3
              1. Nf3
              1. ♘g1f3
              1. ♘f3
            - Notation supports branching variations
              1. e4
              [ 1. d4 d5 2. c4
                (2. Nf3)
              ]
              1... e5
              [ 2... c5]
    - Current move is always highlighted
    - User can click on any move on the notation widget and current
      state will be set to selected move
    - User can scroll through notation with mouse wheel or arrow buttons
    - User can copy/paste [FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) and copy/paste/load/save [PGN](https://en.wikipedia.org/wiki/Portable_Game_Notation)

## Chess Board
Chess Board is a part of project that is responsible for users ability to perfrom
different manipulations with chess board. It consists of 2 subparts: front-end and
backend.

### Frontend is a collection of widgets that allow user to manipulate chess board.
#### Features implemented:
    - Start new game
    - Enter move via drag-and-drop operation
        this includes castlings, enpassants and pawn promotions
    - Flip board (Needs to be bind to Ctrl+B shortcut)
#### Features to be implemented:
    - highlighted kings square if threatened
    - highlighted threatened and protected squares (as setting)
    - board editor
    - enter move via two sequential clicks
    - ability to draw arrows over the board with right-click drag-and-drop
    - ability to draw arrows for moves got from engine (in analysis mode)
    - [premoves](https://en.wikipedia.org/wiki/Premove)

### Backend is responsible for checking information obtained from frontend
#### Features implemented:
    - Start new game
    - Calculate move validness and perform it
        this includes castlings, enpassants and pawn promotions
        also this includes kings safety rules
    - Flip board
    - Check game end cases:
        - mate
        - stalemate
        - insufficient material
#### Features to be implemented:
    - Check game end case:
        - threefold repetition (may be implemented in Notation part)
