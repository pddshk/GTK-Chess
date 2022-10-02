CC		= clang
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK) $(RSVG)`
LDFLAGS	+= `pkg-config --libs $(GTK) $(RSVG)` -lm
OBJDIR	= obj
MAINOBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o state.o rules.o gui.o)
MANAGEROBJECTS = $(addprefix $(OBJDIR)/, engine_manager.o)
OBJECTS = $(MAINOBJECTS) $(MANAGEROBJECTS)

all: prepare $(OBJECTS)
	$(CC) -Wall -o engine_manager $(MANAGEROBJECTS)
	$(CC) $(CFLAGS) -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)

prepare:
	mkdir -p obj

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cleaner: clean
	rm -rf GTKChess

clean:
	rm -rf $(OBJECTS)

run:
	./GTKChess
