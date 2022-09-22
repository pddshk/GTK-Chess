#include <gtk/gtk.h>
#include <cairo.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"

#include "globals.h"

int main(int argc, char** argv)
{
	pipe(from_engine);
	pipe(to_engine);
	pid_t pid = fork();
	switch (pid) {
		case 0:
			while ((dup2(from_engine[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
			while (dup2(to_engine[0], STDIN_FILENO) == -1) {}
			execl(engine, "", NULL);
			break;
		case -1:
			perror("Fork failed. No engine will be available\n");
		default:
			gtk_init(&argc, &argv);
			init_elements();
			gtk_main();
			break;
	}
    return EXIT_SUCCESS;
}
