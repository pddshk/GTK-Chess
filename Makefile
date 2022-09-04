GTK=gtk+-3.0
GCanvas=goocanvas-2.0
C_FLAGS=-Wall `pkg-config --cflags $(GTK)` `pkg-config --cflags $(GCanvas)`
LD_FLAGS=`pkg-config --libs $(GTK)` `pkg-config --libs $(GCanvas)`
FLAGS=`pkg-config --cflags --libs $(GTK)` `pkg-config --cflags --libs $(GCanvas)`

main:
	clang $(C_FLAGS) src/main.c -o bin/GTKChess $(LD_FLAGS)
	
clean:
	rm -rf bin
	
run:
	bin/GTKChess
