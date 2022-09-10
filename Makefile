CC		= clang
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK) $(RSVG)`
LDFLAGS	+= `pkg-config --libs $(GTK) $(RSVG)` -lm
OBJDIR	= obj
OBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o state.o rules.o)

all: prepare $(OBJECTS)
	$(CC) $(CFLAGS) -o bin/GTKChess $(OBJECTS) $(LDFLAGS)

prepare:
	mkdir -p obj bin

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cleaner: clean
	rm -rf bin/*

clean:
	rm -rf $(OBJECTS)

run:
	bin/GTKChess
