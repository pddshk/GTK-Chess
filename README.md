# GTK-Chess
[![license: WTFPL](https://img.shields.io/badge/license-WTFPL-brightgreen.svg)](http://www.wtfpl.net/about/)
[![documentation: unstable](https://img.shields.io/badge/documentation-unstable-brightgreen)](https://pddshk.github.io/GTK-Chess/)
UCI chess engine GUI written in C.

![Main window](https://github.com/pddshk/GTK-Chess/blob/master/docs/source/main.png?raw=true)

# Currently implemented

- [x] Basic UI
- [x] Enter move via grag-and-drop
- [ ] Enter move via consecutive clicks
- [x] Move vaidation (i.e. game rules)
- [x] Ability to run engine
- [x] Interaction with engine
- [ ] Show engine output on GUI

# Build
Linux only. Requires `gtk+-3.0` and `librsvg-2.0`.

```
make
```

This will produce `GTKChess` executable.
