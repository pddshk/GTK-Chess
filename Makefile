CC		= clang
GTK		= gtk+-3.0
CFLAGS	+= -Wall `pkg-config --cflags $(GTK)`
LDFLAGS	+= `pkg-config --libs $(GTK)` -lm
OBJDIR	= obj
OBJECTS	= $(addprefix $(OBJDIR)/, main.o board.o)

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
