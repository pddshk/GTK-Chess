CC		= clang
GTK		= gtk+-3.0
GCanvas	= goocanvas-3.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK)` `pkg-config --cflags $(GCanvas)`
LDFLAGS	+= `pkg-config --libs $(GTK)` `pkg-config --libs $(GCanvas)` -lm
OBJDIR	= obj
OBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o)

all: prepare $(OBJECTS)
	$(CC) $(CFLAGS) -o bin/GTKChess $(OBJECTS) $(LDFLAGS)

prepare:
	mkdir -p obj

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS)

run:
	bin/GTKChess
