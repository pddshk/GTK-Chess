CC		= clang
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
GIO		= gio-unix-2.0 gio-2.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK) $(RSVG) $(GIO)`
LDFLAGS	+= `pkg-config --libs $(GTK) $(RSVG) $(GIO)` -lm
OBJDIR	= obj
OBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o state.o rules.o gui.o)# engine_manager.o)

all: prepare $(OBJECTS)
	$(CC) $(CFLAGS) -g -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)

prepare:
	mkdir -p obj

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cleaner: clean
	rm -rf $(OBJDIR)/*

clean:
	rm -rf $(OBJECTS)

run:
	./GTKChess
