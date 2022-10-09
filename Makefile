CC		= clang
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
GIO		= gio-unix-2.0 gio-2.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK) $(RSVG) $(GIO)`
LDFLAGS	+= `pkg-config --libs $(GTK) $(RSVG) $(GIO)` -lm
OBJDIR	= obj
MAINOBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o state.o rules.o gui.o)
MANAGEROBJECTS = $(addprefix $(OBJDIR)/, engine_manager.o)
OBJECTS = $(MAINOBJECTS) $(MANAGEROBJECTS)

all: prepare engine_manager $(OBJECTS)
	$(CC) $(CFLAGS) -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)

engine_manager: $(OBJDIR)/engine_manager.o
	$(CC) $(FLAGS) -o engine_manager $< $(LDFLAGS)

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
