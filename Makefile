CC		= clang
STD		?= c17
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
GIO		= gio-unix-2.0 gio-2.0
PKGCONF	= $(shell which pkg-config)
CFLAGS	+= -Wall -std=$(STD) -O3 `$(PKGCONF) --cflags $(GTK) $(RSVG) $(GIO)`
LDFLAGS	+= `$(PKGCONF) --libs $(GTK) $(RSVG) $(GIO)` -lm
OBJDIR	= obj
OBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o state.o rules.o gui.o)

all: prepare engine_manager $(OBJECTS)
	$(CC) $(CFLAGS) -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)
	gio set -t string GTKChess metadata::custom-icon file://$(PWD)/src/textures/classic/WKnight.svg

engine_manager: $(OBJDIR)/engine_manager.o
	$(CC) $(CFLAGS) -o engine_manager $< $(LDFLAGS) -pthread

prepare:
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cleaner: clean
	rm -f GTKChess

clean:
	rm -f $(OBJECTS)

run:
	./GTKChess
