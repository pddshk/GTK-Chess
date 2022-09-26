#include <gtk/gtk.h>
#include <cairo.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "gui.h"
#include "engine.h"

#include "globals.h"

int main(int argc, char** argv)
{
	printf("errno is %s\n", strerror(errno));
	int from_engine_fd[2], to_engine_fd[2];
	pipe(from_engine_fd);
	pipe(to_engine_fd);
	pid_t pid = fork();
	switch (pid) {
		case 0:
			close(to_engine_fd[1]); // close to engine write end
			close(from_engine_fd[0]); // close from engine read end
			printf("errno is %d\n", errno);
			while (dup2(to_engine_fd[0], STDIN_FILENO) == -1 && (errno == EINTR));
			close(to_engine_fd[0]);
			while ((dup2(from_engine_fd[1], STDOUT_FILENO) == -1) && (errno == EINTR));
			close(from_engine_fd[1]);

			execl(engine, "", NULL);
			break;
		case -1:
			perror("Fork failed. No engine will be available. Try relaunch application\n");
		default:

			close(from_engine_fd[1]);
			close(to_engine_fd[0]);
			from_engine = from_engine_fd[0];
			to_engine = to_engine_fd[1];
			int flags = fcntl(from_engine, F_GETFL, 0);
			printf("from engine is %d blocked\n", flags & O_NONBLOCK);
			gtk_init(&argc, &argv);
			init_elements();
			//May get stuck at reading from pipe
			//init_engine();
			gtk_main();
			break;
	}
    return EXIT_SUCCESS;
}
