# GTK-Chess
[![license: WTFPL](https://img.shields.io/badge/license-WTFPL-brightgreen.svg)](http://www.wtfpl.net/about/)
[![documentation: latest](https://img.shields.io/badge/documentation-latest-orange)](https://pddshk.github.io/GTK-Chess/)

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
The project is supposed to be built and run on Linux, and all the development process is done under the Linux platform, however it is supposed to run on Windows and Mac as well. Requires `gtk+-3.0` and `librsvg-2.0`.

Feel free to use `make` or `meson`.

## Make

Simple

```
make
```

should produce `GTKChess` executable.

N.B. `make` is using `clang` as default compiler.

## Meson

```
meson setup build
cd build
meson compile
```
