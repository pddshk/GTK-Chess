GTK=gtk+-3.0
C_FLAGS=-Wall `pkg-config --cflags $(GTK)`
LD_FLAGS=`pkg-config --libs $(GTK)`
FLAGS=`pkg-config --cflags --libs $(GTK)`

main:
	gcc $(C_FLAGS) src/main.c -o bin/GTKChess $(LD_FLAGS)
	
clean:
	rm -rf bin
